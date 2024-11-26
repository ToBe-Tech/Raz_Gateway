<!-- instructions push certificate to modem using code: 
- enable in config:  
    modem_key_mgmt (or in build/zephyr/.config CONFIG_MODEM_KEY_MGMT=y)

- copy modemSetup.h to src
- import it from main
- modemSetup.h will search ../cert/DigiCertGlobalRootCA.pem
- copy DigiCertGlobalRootCA.pem to the right place
- in main() before "lte_lc_init_and_connect()":
    err = cert_provision();
	if (err) {
        printk("cert_provision error\n");
		return;
	}
- in main() after "fd = socket(..." before "err = connect(fd...)
    /* Setup TLS socket options */
	err = tls_setup(fd, HTTPS_HOSTNAME); //, sizeof(HTTPS_HOSTNAME
	if (err) {
		goto clean_up;
	}

 -->
