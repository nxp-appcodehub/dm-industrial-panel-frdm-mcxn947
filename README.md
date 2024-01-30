# NXP Application Code Hub
[<img src="https://mcuxpresso.nxp.com/static/icon/nxp-logo-color.svg" width="100"/>](https://www.nxp.com)

## Industrial Panel FRDM-MCXN947

This demo demostrate capabilities of new FRDM-MCXN947. With only one core, demo runs motor control, lvgl, server, two temperature sensors, touch sensitive and get core performance. Include two graphic interfaces, lcd screen and http server, both support interacion and contains same values. Graphic interface in lcd has four screens, that screens allows to watch core performance, microcontroler temperature, board temperature, touch sensitive, manual counter and set rpm in motor. In http server has one page that shows all screens where can do and watch the same as the lcd.


[<img src="Images/CPU_performance.PNG" width="300"/>](Images/CPU_performance.PNG) [<img src="Images/Temperature.PNG" width="300"/>](Images/Temperature.PNG)    
[<img src="Images/Touch.PNG" width="300"/>]() [<img src="Images/Control.PNG" width="300"/>](Images/Control.PNG)   
[<img src="Images/Web_panel.PNG" width="600"/>](Images/Web_panel.PNG)

#### Boards: FRDM-MCXN947
#### Categories: Graphics, Industrial, Motor Control, Sensor
#### Peripherals: ADC, PWM, UART, TIMER, I2C, ETHERNET
#### Toolchains: MCUXpresso IDE, VS Code

## Table of Contents
1. [Software](#step1)
2. [Hardware](#step2)
3. [Setup](#step3)
4. [Results](#step4)
5. [FAQs](#step5) 
6. [Support](#step6)
7. [Release Notes](#step7)

## 1. Software<a name="step1"></a>
- [MCUXpresso 11.9.0 or newer.](https://nxp.com/mcuxpresso)
- [MCUXpresso for VScode 1.5.61 or newer](https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/general-purpose-mcus/lpc800-arm-cortex-m0-plus-/mcuxpresso-for-visual-studio-code:MCUXPRESSO-VSC?cid=wechat_iot_303216)
- [SDK for FRDM-MCXN947.](https://mcuxpresso.nxp.com/en/select)


## 2. Hardware<a name="step2"></a>
- [FRDM-MCXN947.](https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/general-purpose-mcus/mcx-arm-cortex-m/mcx-n94x-and-n54x-mcus-with-dual-core-arm-cortex-m33-eiq-neutron-npu-and-edgelock-secure-enclave-core-profile:MCX-N94X-N54X)   
[<img src="Images/MCXN947.jpg" width="300"/>](Images/MCXN947.jpg)
- [LCD-PAR-S035.](https://www.nxp.com/design/design-center/development-boards/3-5-480x320-ips-tft-lcd-module:LCD-PAR-S035)    
[<img src="Images/LCDNXP.jpg" width="300"/>](Images/LCDNXP.jpg)
- [FRDM-MC-LVPMSM.](https://www.nxp.com/design/design-center/development-boards/freedom-development-boards/mcu-boards/nxp-freedom-development-platform-for-low-voltage-3-phase-pmsm-motor-control:FRDM-MC-LVPMSM)
- [FRDM-MC-LVMTR.](https://www.nxp.com/design/design-center/development-boards/freedom-development-boards/mcu-boards/low-voltage-3-phase-motor-for-frdm-platform:FRDM-MC-LVMTR)   
[<img src="Images/Driver_motor.jpg" width="300"/>](Images/Driver_motor.jpg)
- Power supply 24 volts 5 amperes
- USB Type-C cable.
- Personal computer.


## 3. Setup<a name="step3"></a>

### 3.1 Step 1
1. Open MCUXpresso IDE, in the Quick Start Panel, choose Import from Application Code Hub   
[<img src="Images/import_project_1.png" width="300"/>](Images/import_project_1.png)

2. Enter the demo name in the search bar.    
[<img src="Images/import_project_2.png" width="300"/>](Images/import_project_2.png)
3. Click Copy GitHub link, MCUXpresso IDE will automatically retrieve project attributes, then click Next>.    
[<img src="Images/import_project_3.png" width="300"/>](Images/import_project_3.png)
4. Select main branch and then click Next>, Select the MCUXpresso project, click Finish button to complete import.    
[<img src="Images/import_project_4.png" width="300"/>](Images/import_project_4.png)



### 3.2 Connect hardware
1. Prepare motor control shield board. There is one signal shared between the LCD header and the Arduino header, is the third pin of J1. You need to fold it in order to avoid seeing interference in the display.   
[<img src="Images/fold_pin_motor_control.PNG" width="300"/>](Images/fold_pin_motor_control.png)
2. Connect the Motor cables to the motor control shield as the picture below. Motor sensors are not required for this demo.    
[<img src="Images/motor_connection.PNG" width="200"/>](Images/motor_connection.PNG)
3. Plugin the LCD-PAR-S025 board on J8. LCD-PAR-S025 has two extra rows that will not match with the J8 of the FRDM-MCXN947 board so make you it is connected properly as the picture below.   
[<img src="Images/lcd_connection.PNG" width="400"/>](Images/lcd_connection.PNG)
4. Assembly the motor control shield on top of the FRDM-MCXN947.
5. Plugin Ethernet (J16)  and USB type-C (J17) cables to the FRDM-MCXN947 board and connect the other sides to the host computer.
6. Connect 24v power supply to the motor control shield and press the reset button on the FRM-MCXN947 board. 
### 3.3 Configure network settings on your computer
1. You will need to change the ethernet network settings on your computer. On Windows machines open Windows settings -> Network & Internet -> Ethernet.   
[<img src="Images/network_config.PNG" width="200"/>](Images/network_config.PNG)
2. Select the Ethernet device you have connected to the FRDM-MCXN947 board and edit the IP settings.
3. Save configuration.
### 3.4 Run Demo
1. Build project.
2. Connect FRDM-MCXN947 to computer with USB Type-C
3. Upload code to FRDM-MCXN947
4. If you have open your internet browser, close it.
5. Push reset button on the FRDM-MCXN947 board and now motor will be run.
5. Wait five seconds.
6. Open you browser and put next link 192.168.0.102/index.html


## 4. Results<a name="step4"></a>
- Change between screens, test sensors, modify motor rpm and try interaction of web page and screens.

## 5. FAQs<a name="step5"></a>
No FAQs have been identified for this project.

## 6. Support<a name="step6"></a>

#### Project Metadata
<!----- Boards ----->
[![Board badge](https://img.shields.io/badge/Board-FRDM&ndash;MCXN947-blue)](https://github.com/search?q=org%3Anxp-appcodehub+FRDM-MCXN947+in%3Areadme&type=Repositories)

<!----- Categories ----->
[![Category badge](https://img.shields.io/badge/Category-GRAPHICS-yellowgreen)](https://github.com/search?q=org%3Anxp-appcodehub+graphics+in%3Areadme&type=Repositories) [![Category badge](https://img.shields.io/badge/Category-INDUSTRIAL-yellowgreen)](https://github.com/search?q=org%3Anxp-appcodehub+industrial+in%3Areadme&type=Repositories) [![Category badge](https://img.shields.io/badge/Category-MOTOR%20CONTROL-yellowgreen)](https://github.com/search?q=org%3Anxp-appcodehub+motor_control+in%3Areadme&type=Repositories) [![Category badge](https://img.shields.io/badge/Category-SENSOR-yellowgreen)](https://github.com/search?q=org%3Anxp-appcodehub+sensor+in%3Areadme&type=Repositories)

<!----- Peripherals ----->
[![Peripheral badge](https://img.shields.io/badge/Peripheral-ADC-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+adc+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-PWM-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+pwm+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-UART-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+uart+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-TIMER-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+timer+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-I2C-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+i2c+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-ETHERNET-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+ethernet+in%3Areadme&type=Repositories)

<!----- Toolchains ----->
[![Toolchain badge](https://img.shields.io/badge/Toolchain-MCUXPRESSO%20IDE-orange)](https://github.com/search?q=org%3Anxp-appcodehub+mcux+in%3Areadme&type=Repositories) [![Toolchain badge](https://img.shields.io/badge/Toolchain-VS%20CODE-orange)](https://github.com/search?q=org%3Anxp-appcodehub+vscode+in%3Areadme&type=Repositories)

Questions regarding the content/correctness of this example can be entered as Issues within this GitHub repository.

>**Warning**: For more general technical questions regarding NXP Microcontrollers and the difference in expected funcionality, enter your questions on the [NXP Community Forum](https://community.nxp.com/)

[![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/@NXP_Semiconductors)
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/nxp-semiconductors)
[![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/nxpsemi/)
[![Follow us on Twitter](https://img.shields.io/badge/Twitter-Follow%20us%20on%20Twitter-white.svg)](https://twitter.com/NXP)

## 7. Release Notes<a name="step7"></a>
| Version | Description / Update                           | Date                        |
|:-------:|------------------------------------------------|----------------------------:|
| 1.0     | Initial release on Application Code Hub        | January 30<sup>th</sup> 2024 |

