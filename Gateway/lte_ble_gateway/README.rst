.. _ToBe_Gateway:

nRF Thingy91: ToBe BLE-LTE Gateway
###########################

.. contents::
   :local:
   :depth: 2

Capabilities Overview
*********

* Periodically collecting data via BLE 5 long range (PHY Coded) from ToBe peripheral devices.
* Transmitting data via HTTP to an API endpoint.
* Reading queued commands from an API endpoint, and execusting said commands to the peripherals.
* Switching to peripheral (with BLE 4) on button click and exchanging information and queued commands from the cloud with a mobile app.
* Offline payload aggregation.
* GPS.
* Low power (Ideally 10-20 uA), unless periodically woken up for BLE-LTE work or interrupted by a button press..

The peripheral data is collected using Bluetooth® Low Energy, Phy coded.
Therefore, this sample acts as a gateway between the Bluetooth LE and the LTE connections to the API endpoints.

Requirements
************

The project supports the following development kit:

.. table-from-rows:: /includes/sample_board_rows.txt
   :header: heading
   :rows: nrf9160dk_nrf9160_ns
   :rows: Thingy91_nrf9160_ns

The project also requires a `Nordic nRF52840 peripheral_. Advertising via BLE 5 long range.

.. include:: /includes/spm.txt

The project runs on the nRF connect SDK - Version 1.90

Building and running
********************

.. |sample path| replace:: :file:`samples/nrf9160/lte_ble_gateway`

.. include:: /includes/build_and_run_nrf9160.txt


Programming the sample
======================

When you connect the nRF9160 development kit to your computer, three virtual serial ports of the USB CDC class should become available:

* The first port is connected to the *main controller* on the development kit, the Thingy91.
* The second port is connected to the *board controller* on the development kit, the nRF52840.

You must program the *board controller* with the :ref:`bluetooth-hci-lpuart-sample` sample first, before programming the main controller with the LTE Sensor Gateway sample application.
Program the board controller as follows:

1. Set the board selection switch in the **NRF52** position.
#. Build the :ref:`hci_lpuart` project in the main 'Gateway' project folder for the thingy91_nrf52840 build target and program the board controller with it.
#. Verify that the programming was successful.
   Use a terminal emulator, like PuTTY, to connect to the second serial port and check the output.
   See :ref:`putty` for the required settings.

After programming the board controller, you must program the main controller with the lte-ble-gateway project.
Program the main controller as follows:

1. Set the board selection switch in the **NRF91** position.
#. Edit build configuration on the lte-ble-gateway project with the thingy91_nrf9160_ns as the build target, add the thingy91_nrf9160_ns.conf fragment and build. and program the main controller with it.
#. Verify that the program was successful.
   To do so, use a terminal emulator, like PuTTY, to connect to the first serial port and check the output.
   See :ref:`putty` for the required settings.

Testing
=======

After programming the main controller with the sample, test it by performing the following steps:

1. Power on your Thingy91 and observe logs via a terminal.
#. An initial BLE scan is suppose to begin, after which a periodic POST command is triggered, which triggers the next BLE scan (usually 24 hours later).
#. If you have a ToBe peripheral, you can also observe the on-discovery flow. 

Dependencies
************

This sample uses the following |NCS| libraries:

* :ref:`lib_nrf_cloud`
* ``drivers/gps_sim``
* ``drivers/sensor/sensor_sim``
* :ref:`dk_buttons_and_leds_readme`
* :ref:`lte_lc_readme`
* :ref:`uart_nrf_sw_lpuart`

It uses the following `sdk-nrfxlib`_ library:

* :ref:`nrfxlib:nrf_modem`

It uses the following Zephyr library:

* :ref:`zephyr:bluetooth_api`

It also uses the following samples:

* :ref:`secure_partition_manager`
* :ref:`bluetooth-hci-lpuart-sample`
