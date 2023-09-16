/**
***************************************************************
* @file ~/csse4011/repo/ahu_drivers_ahu/bsu_base.c
* @author Riley Mosca - 45358195
* @date 20 - 03 - 2023
* @brief Source file which handles the bluetooth communications 
*        between the nrf52840 and the ESP32-C3 DevKitM.
***************************************************************
*/

#include "bsu_base.h"
#include "snode_linked_list.h"

static void start_scan(void);

/*
    Log Module Register for the BSU
*/
LOG_MODULE_REGISTER(BSU_LOG, 4);

/*
    Default bt_conn struct pointer
    for default connections
*/
static struct bt_conn *default_conn;

/*
    BT Unique Universal ID pointer 
    that maps to the  HCI_AHU UUID
*/
static struct bt_uuid *bt_uuid_ptr = HCI_AHU_UUID;

/*
    boolean, that tracks connection state
*/
bool ble_connected;

/*
    Base Station Unit UUID
    for this device
*/
uint16_t bsu_uuid[] = {0xAA, 0x43, 0x54, 0xDD, 0xFD, 0x4E, 0x89, 0xBE, 0x32, 0x22, 0xFF, 0x13, 0x3F, 0x2A, 0x29, 0x95};

/*
    Bluetooth write handle,
    used later to write to
    the AHU
*/
static uint16_t bluetooth_write_handle;

/*
    Global device ID tracker
    to ensure that unexpected
    devices that don't match TX
    packet data are ignored
*/
int desired_dev_id;

/*
    Global JSON trackers
    for position and all
    floating point values
*/
int tracker = 0;
float orientation_roll = 0.0;
float orientation_pitch = 0.0;
float altitude_fine = 0.0;
float altitude_coarse = 0.0;
int prac2 = 0;    
int vals[8];

/*
    Data parse callback: Checks for valid UUID and creates
    a connection
*/
static bool data_parse_cb(struct bt_data *data, void *user_data) {

    bt_addr_le_t *addr = user_data;
    int count = 0;

    if (data->type == BT_DATA_UUID128_ALL) {
        for (int i = 0; i < data->data_len; i++) {
            if (data->data[i] == bsu_uuid[i]) {
                count++;
            }
        }
        if (count == UUID_BUFFER_SIZE) {
            int err;
            err = bt_le_scan_stop();
            k_msleep(10);
            if (err) {
                printk("Scan Stop Failed\r\n");
                return true;
            }
            struct bt_le_conn_param *connectionParams = BT_LE_CONN_PARAM_DEFAULT;
            err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN, connectionParams, &default_conn);
            if (err) {
                printk("Connection failed\r\n");
                start_scan();
            }
            return false;
        }
    }
    return true;
}
/*
    Device found callback
*/
static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
			 struct net_buf_simple *ad) {

	char addr_str[BT_ADDR_LE_STR_LEN];
	int err;
    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));

	//Exit if device is not conectable
	if (type != BT_GAP_ADV_TYPE_ADV_IND &&
	    type != BT_GAP_ADV_TYPE_ADV_DIRECT_IND) {
		return;
	}

    bt_data_parse(ad, data_parse_cb, (void*) addr);
}

/**
 * @brief Start an active BLE scan for nearby devices.
 */
static void start_scan(void) {
	int err;
    
    struct bt_le_scan_param btScanParams = {
        .type = BT_LE_SCAN_TYPE_ACTIVE,
        .options = BT_LE_SCAN_OPT_NONE,
        .interval = BT_GAP_SCAN_FAST_INTERVAL,
        .window = BT_GAP_SCAN_FAST_WINDOW,
    };

    //Start scanning
	err = bt_le_scan_start(&btScanParams, device_found);
	if (err) {
		printk("Scanning failed to start (err %d)\n", err);
		return;
	}

	printk("Scanning successfully started\n");
}

/**
 * @brief Callback function for a gatt discovery of a BLE connection
 * @param conn connection handle 
 * @param attr attribute data for connection
 * @param params parameters of gatt discovery
 * @return uint8_t
 */
static uint8_t gatt_discover_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			     struct bt_gatt_discover_params *params) {
	int err;

	if (attr == NULL) {
		if (bluetooth_write_handle == 0) {
            LOG_ERR("Did not discover AHU GATT");
		}
		memset(params, 0, sizeof(*params));
		return BT_GATT_ITER_STOP;
	}

    LOG_DBG("Attribute handle %u", attr->handle);

	if (params->type == BT_GATT_DISCOVER_PRIMARY && bt_uuid_cmp(params->uuid, HCI_AHU_UUID) == 0) {
		LOG_DBG("Found primary service");

		params->uuid = NULL;
		params->start_handle = attr->handle + 1;
		params->type = BT_GATT_DISCOVER_CHARACTERISTIC;

		err = bt_gatt_discover(conn, params);
		if (err != 0) {
            LOG_ERR("Discovery of GATT Characteristics failed (err %d)", err);
		}

		return BT_GATT_ITER_STOP;
	} else if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC) {
		struct bt_gatt_chrc *chrcGatt = (struct bt_gatt_chrc *) attr->user_data;

        err= bt_uuid_cmp(chrcGatt->uuid, HCI_CHAR_AHU_UUID);
		if (!err) {
            LOG_DBG("Found AHU Characteristic");

			bluetooth_write_handle = chrcGatt->value_handle;
		} 
	}

	return BT_GATT_ITER_CONTINUE;
}


/**
 * @brief Discover the GATT services and characteristics of a BLE connection
 */
static void gatt_discover(void) {
	static struct bt_gatt_discover_params gattDiscoverParams;
	int err;

    LOG_DBG("Begin discovering services and characteristics");

    gattDiscoverParams.type = BT_GATT_DISCOVER_PRIMARY;
	gattDiscoverParams.uuid = bt_uuid_ptr;
	gattDiscoverParams.func = gatt_discover_cb;
	gattDiscoverParams.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
	gattDiscoverParams.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;

	err = bt_gatt_discover(default_conn, &gattDiscoverParams);
	if (err != 0) {
        LOG_ERR("Discovery of gatt services and characteristics failed");
        return;
    }

	LOG_DBG("Discovery complete");
}

/**
 * @brief BLE connection callback function
 * @param conn connection handle
 * @param err error code
 */
static void connected(struct bt_conn *conn, uint8_t err) {
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (err != 0) {
		LOG_ERR("Failed to connect to %s (%u)", addr, err);

		bt_conn_unref(default_conn);
		default_conn = NULL;

		start_scan(); 
		return;
	}

	LOG_DBG("Connected to %s", addr);

	default_conn = conn;
    ble_connected = true;

	LOG_DBG("Discovering services and characteristics");

	gatt_discover();
}

/**
 * @brief BLE disconnected callback function
 * @param conn connection handle
 * @param reason reason for disconnection
 */
static void disconnected(struct bt_conn *conn, uint8_t reason) {
	char addr[BT_ADDR_LE_STR_LEN];

	if (conn != default_conn) {
		return;
	}

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_ERR("Disconnected: %s (reason 0x%02x)\n", addr, reason);

	bt_conn_unref(default_conn);
	default_conn = NULL;
    ble_connected = false;

	start_scan();
}

/**
 * @brief gatt write callback function
 * @param conn connection handle]
 * @param err error code
 * @param params gatt parameters
 */
static void hci_write_gatt(struct bt_conn *conn, uint8_t err, struct bt_gatt_write_params *params) {

	memset(params, 0, sizeof(*params));

    return;
}

/**
 * @brief callback function to read an incoming message over gatt
 * @param conn connection handle
 * @param attr attributes of gatt
 * @param buf incoming dat 
 * @param len data length
 * @param flags set flags
 */
static void hci_read_gatt(struct bt_conn *conn, const struct bt_gatt_attr *attr, 
                const uint8_t *buf, uint16_t len, uint8_t flags) {
    /*
        Casting the receive buffer 
        to a HCI Packet pointer 
    */
    hci_packet* rxPack = (hci_packet *) buf;
    static_node* nodeCheck;

    /* Positional indexer */
    int listIdx = 0;
    int posIdx = 0;

    /* If node exists in the list, return, as we cannot add it*/
    SYS_DLIST_FOR_EACH_CONTAINER(&node_list, nodeCheck, node) {
        posIdx = (int)(toupper(nodeCheck->node_name[5]) - 65);
        vals[listIdx] = rxPack->msg_data[posIdx];
        ++listIdx; 
    }

    /* 
        Setting up float, for memcpy later
        int storing current device ID 
    */
    float val;
    memcpy(&val, rxPack->msg_data, sizeof(float));

    int time = 0;

    if(1) {

        printk(
            "%d, "\
            "%d, "\
            "%d, "\
            "%d, "\
            "%d, "\
            "%d, "\
            "%d, "\
            "%d\r\n",
            vals[0],
            vals[1],
            vals[2],
            vals[3],
            vals[4],
            vals[5],
            vals[6],
            vals[7]
        );
    }

    if(prac2 == 1) {

        /* Printing received device values and sending to shell*/
        printk(
            "%d, "\
            "%d, "\
            "%d, "\
            "%d, "\
            "%d, "\
            "%d, "\
            "%d, "\
            "%d, "\
            "%d, "\
            "%d, "\
            "%d, "\
            "%d\r\n",
            rxPack->msg_data[0],
            rxPack->msg_data[1],
            rxPack->msg_data[2],
            rxPack->msg_data[3],
            rxPack->msg_data[4],
            rxPack->msg_data[5],
            rxPack->msg_data[6],
            rxPack->msg_data[7],
            rxPack->msg_data[8],
            rxPack->msg_data[9],
            rxPack->msg_data[10],
            rxPack->msg_data[11]
        );
        switch(rxPack->msg_dev_id) {
            case ROLL_DEV:
                memcpy(&orientation_roll, rxPack->msg_data, sizeof(float));
                tracker++;
                break;
            case PITCH_DEV:
                memcpy(&orientation_pitch, rxPack->msg_data, sizeof(float));
                tracker++;
                break;
            case FIN_ALT_DEV:
                memcpy(&altitude_fine, rxPack->msg_data, sizeof(float));
                // printk("Alt fine = %f", altitude_fine);
                tracker++;
                break;
            case COA_ALT_DEV:
                memcpy(&altitude_coarse, rxPack->msg_data, sizeof(float));
                tracker++;
                break;
        }

        if((desired_dev_id == CON_SMP_DEV) && (tracker == 4)) {
                tracker = 0;
                time = k_uptime_get();
                printk("{\r\n");
                printk("\t<%d> [%f],\r\n", ROLL_DEV, orientation_roll);
                printk("\t<%d> [%f],\r\n", PITCH_DEV, orientation_pitch);
                printk("\t<%d> [%f],\r\n", FIN_ALT_DEV, altitude_fine);
                printk("\t<%d> [%f],\r\n", COA_ALT_DEV, altitude_coarse);
                printk("\t<time> [%d] ms\r\n", time);
                printk("}\r\n");
        }
        
        /*
            We only care about packets that
            match the request!
        */
        if((rxPack->msg_dev_id == desired_dev_id) &&
                (rxPack->msg_dev_id != NO_DEV) &&
                (rxPack->msg_dev_id != CON_SMP_DEV)) {
                    
            memcpy(&val, rxPack->msg_data, sizeof(float));
            printk("<DID: %d> [%f]\r\n", rxPack->msg_dev_id, val);
            
        }
    }

    return;
}
/*
    Checks to see that a valid BSU device is used
*/
bool bsu_valid_device(int device_id) {

    int valid_ids[] = {COA_ALT_DEV, FIN_ALT_DEV, PITCH_DEV, ROLL_DEV, ULTRAS_DEV};
    bool valid = false;

    for(int i = 0; i < 5; i++) {
        if(valid_ids[i] == device_id) {
            valid = true;
        }
    }
    return valid;
}

/**
 * @brief function to send a hci data packet over BLE
 * @param handle handle of gatt characteristic
 * @param pack data packet to send over BLE
 */
static void gatt_write(uint16_t handle, hci_packet *pack) {

	static struct bt_gatt_write_params params;
	int err;
    char *buf = (char *) pack;

    params.data = buf;
    params.length = sizeof(hci_packet);
	params.func = hci_write_gatt;
	params.handle = handle;

	err = bt_gatt_write(default_conn, &params);
	if (err != 0) {
		LOG_ERR("Failed write (err 0x%X)", err);
	}
}

/**
 * @brief define a new service for the bsu along with corresponding characteristics
 */
BT_GATT_SERVICE_DEFINE(bsu_ahu_service, BT_GATT_PRIMARY_SERVICE(HCI_BSU_UUID),
	BT_GATT_CHARACTERISTIC(HCI_CHAR_BSU_UUID,
    			    (BT_GATT_CHRC_WRITE_WITHOUT_RESP),
			       (BT_GATT_PERM_WRITE), NULL,
			       hci_read_gatt, NULL),
);

/**
 * @brief define the connection callback functions for BLE
 */
BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

/**
 * @brief Main thread for Base Station Unit's (BSU command) command line interface implementation
 */
void bsu_shell_thread(void) {

    /* 
        Enabling Bluetooth, catching error on failure 
    */
    int err = bt_enable(NULL);

    /*
        Initialising a HCI Packet to populate
    */
    hci_packet tx_packet_prev;
    hci_packet tx_packet_curr;

    /*
        return on error
    */
    if (err) {
        LOG_ERR("Bluetooth init failed (err %d)\r\n", err);
        return;
    }

    /* 
        Successfully initialised Bluetooth!
    */
    LOG_DBG("Bluetooth initialised\r\n");
    extern struct k_msgq packet_queue;
    

    /*
        Cyclic executive for main BT Thread
    */
    for (;;) {
        
        /*
            Checking message queue, waiting on HCI Packets
            from shell (based on shell command usage) then
            writing these packets to bluetooth
        */
        if((ble_connected)) {

            if (k_msgq_get(&packet_queue, &tx_packet_curr, K_MSEC(10)) == 0) {
                /*
                    Setting desired Device ID
                */
                desired_dev_id = tx_packet_curr.msg_dev_id;

                /*
                    Writing messages to bluetooth as they are received
                */
                gatt_write(bluetooth_write_handle, &tx_packet_curr);
            }
        }
        tx_packet_prev = tx_packet_curr;
        
        /*
            Sleep for 50ms before 
            processing another message
        */
        k_msleep(100);
    }
}

/*
    NRF52840 LED Thread

    - Used to indicate a bluetooth 
    connection has been established,
    with blue LED for connected,
    else the LED is purple
*/
void nrf_52840_bluetooth_led_thread(void)
{

    /*
        Connection boolean: Blue on true, else purple
        Led active boolean: True is on, else off
    */
    ble_connected = false;
    bool active = true;

    /*
        Configuring GPIO's for the LED's on the NRF (LED0 and LED1)
    */
    gpio_pin_configure(device_get_binding(LED0), PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
    gpio_pin_configure(device_get_binding(LED1), PIN1, GPIO_OUTPUT_ACTIVE | FLAGS1);

    while (1) {
        /*
            Flipping states, so the LED blinks
        */
        active = !active;

        if (ble_connected) {
            gpio_pin_set(device_get_binding(LED1), PIN1, (int) active);
            gpio_pin_set(device_get_binding(LED0), PIN, (int) false);
            k_msleep(BLED_LED_SLEEP);

        }

        else {
            gpio_pin_set(device_get_binding(LED1), PIN1, (int) active);
            gpio_pin_set(device_get_binding(LED0), PIN, (int) active);
            k_msleep(BLED_LED_SLEEP);
        }
    }
}

/*
    bsu_scan: Function that toggles scanning for BTLE devices
    usage: bsu_scan < o | f >
*/
void bsu_scan_cmd(const struct shell *shell, size_t argc, char **argv) {
    /* 
        Checking the current command state
    */
    char* scan_state = argv[1];

    /*
        Starting/ Stopping based on state
    */
    if(strcmp(scan_state, "o") == 0) start_scan();
    else if(strcmp(scan_state, "f") == 0) bt_le_scan_stop();

    /*
        Formatting debug info, based on state
    */
    char* scan_str = !strcmp(scan_state, "o") ? 
        "Scanning has started!\r\n" :
        "Scanning has stopped!\r\n";
}


/* Thread Define - NRF LED for BT */
K_THREAD_DEFINE(bsu_led_thread_id, BSU_LED_THREAD_STACK, nrf_52840_bluetooth_led_thread, NULL, NULL, NULL, BSU_LED_THREAD_PRIORITY, 0, 0);

/* Thread Define - Bluetooth main thread */
K_THREAD_DEFINE(bsu_shell_thread_tid, BSU_BT_THREAD_STACK, bsu_shell_thread, NULL, NULL, NULL, BSU_BT_THREAD_PRIORITY, 0, 0);

/* BSU SCAN Command registration */
SHELL_CMD_ARG_REGISTER(bsu_scan, NULL, BSU_SCAN_INFO_MSG, bsu_scan_cmd, BSU_SCAN_MAND_ARGS, BSU_SCAN_OPT_ARGS);