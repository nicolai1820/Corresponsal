/*
* API
* Version: V6.5.0
*/

#include <stdio.h>
#include <time.h>
#include <sys/socket.h>

#ifndef __NDKAPI__H
#define __NDKAPI__H

/** @addtogroup CommonDefinitions
* @{
*/

typedef void *NDK_HANDLE; /**<File descriptor*/
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

/** @}*/ // End of common definitions

/** @addtogroup ErrorCodes
* @{
*/

/**
 *@brief Error codes
*/
typedef enum {
        NDK_OK,                                /**<Success*/
        NDK_ERR                        = -1,   /**<Fail*/
        NDK_ERR_INIT_CONFIG            = -2,   /**<Failed to initialize configuration*/
        NDK_ERR_CREAT_WIDGET           = -3,   /**<Failed to error creating interface*/
        NDK_ERR_OPEN_DEV               = -4,   /**<Failed to error opening device file*/
        NDK_ERR_IOCTL                  = -5,   /**<Failed to call driver function*/
        NDK_ERR_PARA                   = -6,   /**<Invalid parameter*/
        NDK_ERR_PATH                   = -7,   /**<Invalid file path*/
        NDK_ERR_DECODE_IMAGE           = -8,   /**<Failed to decode image*/
        NDK_ERR_MACLLOC                = -9,   /**<Out of memory*/
        NDK_ERR_TIMEOUT                = -10,  /**<Timeout error*/
        NDK_ERR_QUIT                   = -11,  /**<Press Cancel to exit*/
        NDK_ERR_WRITE                  = -12,  /**<Failed to write into file*/
        NDK_ERR_READ                   = -13,  /**<Failed to read from file*/
        NDK_ERR_OVERFLOW               = -15,  /**<Buffer overflow*/
        NDK_ERR_SHM                    = -16,  /**<Failed to share memory*/
        NDK_ERR_NO_DEVICES             = -17,  /**<Device not available*/
        NDK_ERR_NOT_SUPPORT            = -18,  /**<Feature not supported*/
        NDK_ERR_NOSWIPED               = -50,  /**<No magnetSmart card swiping*/
        NDK_ERR_SWIPED_DATA            = -51,  /**<Wrong magnetSmart card data*/
        NDK_ERR_USB_LINE_UNCONNECT     = -100, /**<Usb cable not connected*/
        NDK_ERR_NO_SIMCARD             = -201, /**<No SIM card*/
        NDK_ERR_PIN                    = -202, /**<Wrong SIM card password*/
        NDK_ERR_PIN_LOCKED             = -203, /**<SIM card locked*/
        NDK_ERR_PIN_UNDEFINE           = -204, /**<Undefined SIM card error*/
        NDK_ERR_EMPTY                  = -205, /**<Empty string returned*/
        NDK_ERR_ETH_PULLOUT            = -250, /**<Ethernet cable not plugged*/

        NDK_ERR_PPP_PARAM              = -301, /**<Invalid PPP parameter*/
        NDK_ERR_PPP_DEVICE             = -302, /**<Invalid PPP device*/
        NDK_ERR_PPP_OPEN               = -303, /**<PPP already open*/

        NDK_ERR_TCP_ALLOC              = -304, /**<Failed to allocate*/
        NDK_ERR_TCP_PARAM              = -305, /**<Invalid parameter*/
        NDK_ERR_TCP_TIMEOUT            = -306, /**<Transmission timeout*/
        NDK_ERR_TCP_INVADDR            = -307, /**<Invalid address*/
        NDK_ERR_TCP_CONNECT            = -308, /**<No connection*/
        NDK_ERR_TCP_PROTOCOL           = -309, /**<Protocol error*/
        NDK_ERR_TCP_NETWORK            = -310, /**<Network error*/
        NDK_ERR_TCP_SEND               = -311, /**<Failed to send*/
        NDK_ERR_TCP_RECV               = -312, /**<Failed to receive*/

        NDK_ERR_WLM_SEND_AT_FAIL       = -320, /**<Failed to transmit AT*/

        NDK_ERR_SSL_PARAM              = -350, /**<Invalid parameter*/
        NDK_ERR_SSL_ALREADCLOSE        = -351, /**<Connection already closed*/
        NDK_ERR_SSL_ALLOC              = -352, /**<Failed to allocate*/
        NDK_ERR_SSL_INVADDR            = -353, /**<Invalid address*/
        NDK_ERR_SSL_TIMEOUT            = -354, /**<Connection Timeout*/
        NDK_ERR_SSL_MODEUNSUPPORTED    = -355, /**<Mode not supported*/
        NDK_ERR_SSL_SEND               = -356, /**<Failed to send*/
        NDK_ERR_SSL_RECV               = -357, /**<Failed to receive*/
        NDK_ERR_SSL_CONNECT            = -358, /**<No connection*/

        NDK_ERR_NET_GETADDR            = -401, /**<Failed to obtain local address or subnet mask*/
        NDK_ERR_NET_GATEWAY            = -402, /**<Failed to obtain gateway address*/
        NDK_ERR_NET_ADDRILLEGAL        = -403, /**<Failed to obtain address format*/
        NDK_ERR_NET_UNKNOWN_COMMTYPE   = -404, /**<Unknown type of communication*/
        NDK_ERR_NET_INVALIDIPSTR       = -405, /**<Invalid IP string*/
        NDK_ERR_NET_UNSUPPORT_COMMTYPE = -406, /**<Type of communication not supported*/

        NDK_ERR_THREAD_PARAM           = -450, /**<Invalid address*/
        NDK_ERR_THREAD_ALLOC           = -451, /**<Failed to allocate*/
        NDK_ERR_THREAD_CMDUNSUPPORTED  = -452, /**<Command not supported*/

        NDK_ERR_MODEM_RESETFAIL        = -501, /**<Failed to reset*/
        NDK_ERR_MODEM_GETSTATUSFAIL    = -502, /**<Failed to get status*/
        NDK_ERR_MODEM_SLEPPFAIL        = -503, /**<Failed to sleep*/
        NDK_ERR_MODEM_SDLCINITFAIL     = -504, /**<Failed to initialize in sync mode*/
        NDK_ERR_MODEM_INIT_NOT         = -505, /**<Not initialized*/
        NDK_ERR_MODEM_SDLCWRITEFAIL    = -506, /**<Failed to write in sync mode*/
        NDK_ERR_MODEM_ASYNWRITEFAIL    = -507, /**<Failed to write in async mode*/
        NDK_ERR_MODEM_ASYNDIALFAIL     = -508, /**<Failed to dial in async mode*/
        NDK_ERR_MODEM_ASYNINITFAIL     = -509, /**<Failed to initialize in async mode*/
        NDK_ERR_MODEM_SDLCHANGUPFAIL   = -510, /**<Failed to hangup in sync mode*/
        NDK_ERR_MODEM_ASYNHANGUPFAIL   = -511, /**<Failed to hangup in async mode*/
        NDK_ERR_MODEM_SDLCCLRBUFFAIL   = -512, /**<Failed to clear buffer in sync mode*/
        NDK_ERR_MODEM_ASYNCLRBUFFAIL   = -513, /**<Failed to clear buffer in async mode*/
        NDK_ERR_MODEM_ATCOMNORESPONSE  = -514, /**<No response for AT command*/
        NDK_ERR_MODEM_PORTWRITEFAIL    = -515, /**<Failed to write data to modem port*/
        NDK_ERR_MODEM_SETCHIPFAIL      = -516, /**<Failed to set register*/
        NDK_ERR_MODEM_STARTSDLCTASK    = -517, /**<Failed to start SDLC task*/
        NDK_ERR_MODEM_GETBUFFLENFAIL   = -518, /**<Failed to get data lenth*/
        NDK_ERR_MODEM_QUIT             = -519, /**<Hand out*/
        NDK_ERR_MODEM_NOPREDIAL        = -520, /**<No predial*/
        NDK_ERR_MODEM_NOCARRIER        = -521, /**<No carrier*/
        NDK_ERR_MODEM_NOLINE           = -523, /**<No cable*/
        NDK_ERR_MODEM_OTHERMACHINE     = -524, /**<Collision detected*/
        NDK_ERR_MODEM_PORTREADFAIL     = -525, /**<Failed to read data from modem port*/
        NDK_ERR_MODEM_CLRBUFFAIL       = -526, /**<Failed to clear buffer*/
        NDK_ERR_MODEM_ATCOMMANDERR     = -527, /**<AT command error*/
        NDK_ERR_MODEM_STATUSUNDEFINE   = -528, /**<State unrecognized*/
        NDK_ERR_MODEM_GETVERFAIL       = -529, /**<Failed to get version*/
        NDK_ERR_MODEM_SDLCDIALFAIL     = -530, /**<Failed to dial in sync mode*/
        NDK_ERR_MODEM_SELFADAPTFAIL    = -531, /**<Failed to auto-negotiation*/
        NDK_ERR_MODEM_SELFADAPTCANCEL  = -532, /**<Auto-negotiation canceled*/

        NDK_ERR_ICC_WRITE_ERR          = -601, /**<Failed to write*/
        NDK_ERR_ICC_COPYERR            = -602, /**<Failed to copy kernel data*/
        NDK_ERR_ICC_POWERON_ERR        = -603, /**<Failed to powerup*/
        NDK_ERR_ICC_COM_ERR            = -604, /**<Command error*/
        NDK_ERR_ICC_CARDPULL_ERR       = -605, /**<Card not present*/
        NDK_ERR_ICC_CARDNOREADY_ERR    = -606, /**<Card not ready*/

        NDK_ERR_USDDISK_PARAM          = -650, /**<Invalid parameter*/
        NDK_ERR_USDDISK_DRIVELOADFAIL  = -651, /**<Failed to load USB stick or SD card*/
        NDK_ERR_USDDISK_NONSUPPORTTYPE = -652, /**<Type not supported*/
        NDK_ERR_USDDISK_UNMOUNTFAIL    = -653, /**<Failed to mount*/
        NDK_ERR_USDDISK_UNLOADDRIFAIL  = -654, /**<Failed to unload driver*/
        NDK_ERR_USDDISK_IOCFAIL        = -655, /**<Failed to call driver function*/

        NDK_ERR_APP_BASE               = -800, /**<Unknown error*/
        NDK_ERR_APP_NOT_EXIST                = (NDK_ERR_APP_BASE - 1), /**<Application not exist*/
        NDK_ERR_APP_NOT_MATCH                = (NDK_ERR_APP_BASE - 2), /**<Patch not match*/
        NDK_ERR_APP_FAIL_SEC                 = (NDK_ERR_APP_BASE - 3), /**<Failed to access tamper status*/
        NDK_ERR_APP_SEC_ATT                  = (NDK_ERR_APP_BASE - 4), /**<Tamper detected*/
        NDK_ERR_APP_FILE_EXIST               = (NDK_ERR_APP_BASE - 5), /**<Application file already exists*/
        NDK_ERR_APP_FILE_NOT_EXIST           = (NDK_ERR_APP_BASE - 6), /**<Application file not exist*/
        NDK_ERR_APP_FAIL_AUTH                = (NDK_ERR_APP_BASE - 7), /**<Failed to authenticate certificate*/
        NDK_ERR_APP_LOW_VERSION              = (NDK_ERR_APP_BASE - 8), /**<Patch version lower than the application version*/
        NDK_ERR_APP_MAX_CHILD                = (NDK_ERR_APP_BASE - 9), /**<More than maximum number of running applications*/
        NDK_ERR_APP_CREAT_CHILD              = (NDK_ERR_APP_BASE - 10), /**<Failed to create child process*/
        NDK_ERR_APP_WAIT_CHILD               = (NDK_ERR_APP_BASE - 11), /**<Failed to wait for the child to exit*/
        NDK_ERR_APP_FILE_READ                = (NDK_ERR_APP_BASE - 12), /**<Failed to read file*/
        NDK_ERR_APP_FILE_WRITE               = (NDK_ERR_APP_BASE - 13), /**<Failed to write file*/
        NDK_ERR_APP_FILE_STAT                = (NDK_ERR_APP_BASE - 14), /**<Failed to get file information*/
        NDK_ERR_APP_FILE_OPEN                = (NDK_ERR_APP_BASE - 15), /**<Failed to open file*/
        NDK_ERR_APP_NLD_HEAD_LEN             = (NDK_ERR_APP_BASE - 16), /**<Wrong length of the NLD file header information*/
        NDK_ERR_APP_PUBKEY_EXPIRED           = (NDK_ERR_APP_BASE - 17), /**<Public key expired*/
        NDK_ERR_APP_MMAP                     = (NDK_ERR_APP_BASE - 18), /**<Failed to map memory*/
        NDK_ERR_APP_MALLOC                   = (NDK_ERR_APP_BASE - 19), /**<Out of memory*/
        NDK_ERR_APP_SIGN_DECRYPT             = (NDK_ERR_APP_BASE - 20), /**<Failed to decrypt signature data*/
        NDK_ERR_APP_SIGN_CHECK               = (NDK_ERR_APP_BASE - 21), /**<Failed to validate signature data*/
        NDK_ERR_APP_MUNMAP                   = (NDK_ERR_APP_BASE - 22), /**<Failed to unmap memory*/
        NDK_ERR_APP_TAR                      = (NDK_ERR_APP_BASE - 23), /**<Failed to untar data*/
        NDK_ERR_APP_KEY_UPDATE_BAN           = (NDK_ERR_APP_BASE - 24), /**<Key update is prohibited*/
        NDK_ERR_APP_FIRM_PATCH_VERSION       = (NDK_ERR_APP_BASE - 25), /**<Firmware patch version do not match*/
        NDK_ERR_APP_CERT_HAS_EXPIRED         = (NDK_ERR_APP_BASE - 26), /**<Certificate expired*/
        NDK_ERR_APP_CERT_NOT_YET_VALID       = (NDK_ERR_APP_BASE - 27), /**<Invalid certificate*/
        NDK_ERR_APP_FILE_NAME_TOO_LONG       = (NDK_ERR_APP_BASE - 28), /**<File name length larger than 32 bytes*/

        NDK_ERR_SECP_BASE                    = (-1000),                  /**<Unknown error*/
        NDK_ERR_SECP_TIMEOUT                 = (NDK_ERR_SECP_BASE - 1),  /**<Get key value timeout*/
        NDK_ERR_SECP_PARAM                   = (NDK_ERR_SECP_BASE - 2),  /**<Invalid parameter*/
        NDK_ERR_SECP_DBUS                    = (NDK_ERR_SECP_BASE - 3),  /**<DBUS communication error*/
        NDK_ERR_SECP_MALLOC                  = (NDK_ERR_SECP_BASE - 4),  /**<Out of memory*/
        NDK_ERR_SECP_OPEN_SEC                = (NDK_ERR_SECP_BASE - 5),  /**<Failed to open security device*/
        NDK_ERR_SECP_SEC_DRV                 = (NDK_ERR_SECP_BASE - 6),  /**<Failed to call driver function*/
        NDK_ERR_SECP_GET_RNG                 = (NDK_ERR_SECP_BASE - 7),  /**<Failed to get random number*/
        NDK_ERR_SECP_GET_KEY                 = (NDK_ERR_SECP_BASE - 8),  /**<Failed to get key value*/
        NDK_ERR_SECP_KCV_CHK                 = (NDK_ERR_SECP_BASE - 9),  /**<KCV check error*/
        NDK_ERR_SECP_GET_CALLER              = (NDK_ERR_SECP_BASE - 10), /**<Failed to get caller info*/
        NDK_ERR_SECP_OVERRUN                 = (NDK_ERR_SECP_BASE - 11), /**<Overrun*/
        NDK_ERR_SECP_NO_PERMIT               = (NDK_ERR_SECP_BASE - 12), /**<Operation not allowed*/
        NDK_ERR_SECP_TAMPER                  = (NDK_ERR_SECP_BASE - 13), /**<Tamper detected*/

        NDK_ERR_SECVP_BASE                   = (-1100),                  /**<Unknown error*/
        NDK_ERR_SECVP_TIMEOUT                = (NDK_ERR_SECVP_BASE - 1), /**<Get key value timeout*/
        NDK_ERR_SECVP_PARAM                  = (NDK_ERR_SECVP_BASE - 2), /**<Invalid parameter*/
        NDK_ERR_SECVP_DBUS                   = (NDK_ERR_SECVP_BASE - 3), /**<DBUS communication error*/
        NDK_ERR_SECVP_OPEN_EVENT0            = (NDK_ERR_SECVP_BASE - 4), /**<Failed to open event device*/
        NDK_ERR_SECVP_SCAN_VAL               = (NDK_ERR_SECVP_BASE - 5), /**<Scan value out of range*/
        NDK_ERR_SECVP_OPEN_RNG               = (NDK_ERR_SECVP_BASE - 6), /**<Failed to open random number device*/
        NDK_ERR_SECVP_GET_RNG                = (NDK_ERR_SECVP_BASE - 7), /**<Failed to get random number*/
        NDK_ERR_SECVP_GET_ESC                = (NDK_ERR_SECVP_BASE - 8), /**<User cancel*/
        NDK_ERR_SECVP_VPP                    = (-1120),                  /**<Unknown error*/
        NDK_ERR_SECVP_INVALID_KEY            = (NDK_ERR_SECVP_VPP),      /**<Invalid key*/
        NDK_ERR_SECVP_NOT_ACTIVE             = (NDK_ERR_SECVP_VPP - 1),  /**<VPPIs not active*/
        NDK_ERR_SECVP_TIMED_OUT              = (NDK_ERR_SECVP_VPP - 2),  /**<VPP initialization timeout*/
        NDK_ERR_SECVP_ENCRYPT_ERROR          = (NDK_ERR_SECVP_VPP - 3),  /**<Failed to encrypt*/
        NDK_ERR_SECVP_BUFFER_FULL            = (NDK_ERR_SECVP_VPP - 4),  /**<Buffer full*/
        NDK_ERR_SECVP_PIN_KEY                = (NDK_ERR_SECVP_VPP - 5),  /**<Data key pressed, echo "*".*/
        NDK_ERR_SECVP_ENTER_KEY              = (NDK_ERR_SECVP_VPP - 6),  /**<Enter key pressed, process PIN*/
        NDK_ERR_SECVP_BACKSPACE_KEY          = (NDK_ERR_SECVP_VPP - 7),  /**<Backspace key pressed.*/
        NDK_ERR_SECVP_CLEAR_KEY              = (NDK_ERR_SECVP_VPP - 8),  /**<Clear key pressed, remove all the '*'.*/
        NDK_ERR_SECVP_CANCEL_KEY             = (NDK_ERR_SECVP_VPP - 9),  /**<Cancel key pressed.*/
        NDK_ERR_SECVP_GENERALERROR           = (NDK_ERR_SECVP_VPP - 10), /**<Internal error.*/
        NDK_ERR_SECVP_CUSTOMERCARDNOTPRESENT = (NDK_ERR_SECVP_VPP - 11), /**<Smart card not present*/
        NDK_ERR_SECVP_HTCCARDERROR           = (NDK_ERR_SECVP_VPP - 12), /**<Failed to access smart card*/
        NDK_ERR_SECVP_WRONG_PIN_LAST_TRY     = (NDK_ERR_SECVP_VPP - 13), /**<Wrong password, try again*/
        NDK_ERR_SECVP_WRONG_PIN              = (NDK_ERR_SECVP_VPP - 14), /**<Try last time.*/
        NDK_ERR_SECVP_ICCERROR               = (NDK_ERR_SECVP_VPP - 15),  /**<Try too many times*/
        NDK_ERR_SECVP_PIN_BYPASS             = (NDK_ERR_SECVP_VPP - 16),  /**<PIN verification succeed, but PIN length is zero*/
        NDK_ERR_SECVP_ICCFAILURE             = (NDK_ERR_SECVP_VPP - 17),  /**<Fatal error.*/
        NDK_ERR_SECVP_GETCHALLENGE_BAD       = (NDK_ERR_SECVP_VPP - 18),  /**<Response is not 90, 00.*/
        NDK_ERR_SECVP_GETCHALLENGE_NOT8      = (NDK_ERR_SECVP_VPP - 19),  /**<Invalid response length.*/
        NDK_ERR_SECVP_PIN_ATTACK_TIMER       = (NDK_ERR_SECVP_VPP - 20),  /**<PIN attack timer activated*/

        NDK_ERR_SECCR_BASE                   = (-1200),                   /**<Unknown error*/
        NDK_ERR_SECCR_TIMEOUT                = (NDK_ERR_SECCR_BASE - 1),  /**<Get key value timeout*/
        NDK_ERR_SECCR_PARAM                  = (NDK_ERR_SECCR_BASE - 2),  /**<Invalid parameter*/
        NDK_ERR_SECCR_DBUS                   = (NDK_ERR_SECCR_BASE - 3),  /**<DBUS communication error*/
        NDK_ERR_SECCR_MALLOC                 = (NDK_ERR_SECCR_BASE - 4),  /**<Out of memory*/
        NDK_ERR_SECCR_OPEN_RNG               = (NDK_ERR_SECCR_BASE - 5),  /**<Failed to open random number device*/
        NDK_ERR_SECCR_DRV                    = (NDK_ERR_SECCR_BASE - 6),  /**<Failed to call driver function*/
        NDK_ERR_SECCR_KEY_TYPE               = (NDK_ERR_SECCR_BASE - 7),  /**<Wrong key type*/
        NDK_ERR_SECCR_KEY_LEN                = (NDK_ERR_SECCR_BASE - 8),  /**<Wrong key length*/
        NDK_ERR_SECCR_GET_KEY                = (NDK_ERR_SECCR_BASE - 9),  /**<Failed to get key*/

        NDK_ERR_SECKM_BASE                   = (-1300),                   /**<Unknown error*/
        NDK_ERR_SECKM_TIMEOUT                = (NDK_ERR_SECKM_BASE - 1),  /**<Get key value timeout*/
        NDK_ERR_SECKM_PARAM                  = (NDK_ERR_SECKM_BASE - 2),  /**<Invalid parameter*/
        NDK_ERR_SECKM_DBUS                   = (NDK_ERR_SECKM_BASE - 3),  /**<DBUS communication error*/
        NDK_ERR_SECKM_MALLOC                 = (NDK_ERR_SECKM_BASE - 4),  /**<Out of memory*/
        NDK_ERR_SECKM_OPEN_DB                = (NDK_ERR_SECKM_BASE - 5),  /**<Failed to open database*/
        NDK_ERR_SECKM_DEL_DB                 = (NDK_ERR_SECKM_BASE - 6),  /**<Failed to delete database*/
        NDK_ERR_SECKM_DEL_REC                = (NDK_ERR_SECKM_BASE - 7),  /**<Failed to delete record*/
        NDK_ERR_SECKM_INSTALL_REC            = (NDK_ERR_SECKM_BASE - 8),  /**<Failed to install key record*/
        NDK_ERR_SECKM_READ_REC               = (NDK_ERR_SECKM_BASE - 9),  /**<Failed to read key record*/
        NDK_ERR_SECKM_OPT_NOALLOW            = (NDK_ERR_SECKM_BASE - 10), /**<Operation not allowed*/
        NDK_ERR_SECKM_KEY_MAC                = (NDK_ERR_SECKM_BASE - 11), /**<MAC error*/
        NDK_ERR_SECKM_KEY_TYPE               = (NDK_ERR_SECKM_BASE - 12), /**<Wrong key type*/
        NDK_ERR_SECKM_KEY_ARCH               = (NDK_ERR_SECKM_BASE - 13), /**<Wrong key architecture*/
        NDK_ERR_SECKM_KEY_LEN                = (NDK_ERR_SECKM_BASE - 14), /**<Wrong key length*/

        NDK_ERR_RFID_INITSTA            = -2005, /**<RF chip error or not configured*/
        NDK_ERR_RFID_NOCARD             = -2008, /**<No card  0x0D*/
        NDK_ERR_RFID_MULTICARD          = -2009, /**<Multi card detected*/
        NDK_ERR_RFID_SEEKING            = -2010, /**<Failed to seek and activate card*/
        NDK_ERR_RFID_PROTOCOL           = -2011, /**<Not compliant with ISO1444-4 protocol, e.g. M1 card F*/
        NDK_ERR_RFID_NOPICCTYPE         = -2012, /**<Card not set 0x01*/
        NDK_ERR_RFID_NOTDETE            = -2013, /**<Card not detected  0x02*/
        NDK_ERR_RFID_AANTI              = -2014, /**<Type A card collision (Multiple cards exist) 0x03*/
        NDK_ERR_RFID_RATS               = -2015, /**<Type A card RATS processing error 0x04*/
        NDK_ERR_RFID_BACTIV             = -2016, /**<Failed to activate Type B card 0x07*/
        NDK_ERR_RFID_ASEEK              = -2017, /**<Failed to seek type A card (Probably multiple cards exist) 0x0A*/
        NDK_ERR_RFID_BSEEK              = -2018, /**<Failed to seek type B card (Probably multiple cards exist) 0x0B*/
        NDK_ERR_RFID_ABON               = -2019, /**<Both type A and B cards exist 0x0C*/
        NDK_ERR_RFID_UPED               = -2020, /**<Already activated 0x0E*/
        NDK_ERR_RFID_NOTACTIV           = -2021, /**<Not activated*/
        NDK_ERR_RFID_COLLISION_A        = -2022, /**<Type A Card collision*/
        NDK_ERR_RFID_COLLISION_B        = -2023, /**<Type B Card collision*/

        NDK_ERR_MI_NOTAGERR             = -2030, /**<No card,                        0xff*/
        NDK_ERR_MI_CRCERR               = -2031, /**<CRC error,                      0xfe*/
        NDK_ERR_MI_EMPTY                = -2032, /**<Not empty,                      0xfd*/
        NDK_ERR_MI_AUTHERR              = -2033, /**<Failed to authenticate,         0xfc*/
        NDK_ERR_MI_PARITYERR            = -2034, /**<Parity error,                   0xfb*/
        NDK_ERR_MI_CODEERR              = -2035, /**<Receiving code error            0xfa*/
        NDK_ERR_MI_SERNRERR             = -2036, /**<Anti-collision data check error 0xf8*/
        NDK_ERR_MI_KEYERR               = -2037, /**<KEY authentication error        0xf7*/
        NDK_ERR_MI_NOTAUTHERR           = -2038, /**<Not authenticated               0xf6*/
        NDK_ERR_MI_BITCOUNTERR          = -2039, /**<Failed to receive BIT           0xf5*/
        NDK_ERR_MI_BYTECOUNTERR         = -2040, /**<Failed to receive byte          0xf4*/
        NDK_ERR_MI_WriteFifo            = -2041, /**<Failed to write FIFO            0xf3*/
        NDK_ERR_MI_TRANSERR             = -2042, /**<Failed to send                  0xf2*/
        NDK_ERR_MI_WRITEERR             = -2043, /**<Failed to error write           0xf1*/
        NDK_ERR_MI_INCRERR              = -2044, /**<Failed to increment             0xf0*/
        NDK_ERR_MI_DECRERR              = -2045, /**<Failed to decrement             0xef*/
        NDK_ERR_MI_OVFLERR              = -2046, /**<Overflow                        0xed*/
        NDK_ERR_MI_FRAMINGERR           = -2047, /**<Frame error                     0xeb*/
        NDK_ERR_MI_COLLERR              = -2048, /**<Collision detected              0xe8*/
        NDK_ERR_MI_INTERFACEERR         = -2049, /**<Fialed to reset interface       0xe6*/
        NDK_ERR_MI_ACCESSTIMEOUT        = -2050, /**<Receive timeout                 0xe5*/
        NDK_ERR_MI_PROTOCOLERR          = -2051, /**<Protocol error                  0xe4*/
        NDK_ERR_MI_QUIT                 = -2052, /**<Abnormal abortion               0xe2*/
        NDK_ERR_MI_PPSErr               = -2053, /**<PPS operation error             0xe1*/
        NDK_ERR_MI_SpiRequest           = -2054, /**<Failed to request SPI           0xa0*/
        NDK_ERR_MI_NY_IMPLEMENTED       = -2055, /**<Unknown error                   0x9c*/
        NDK_ERR_MI_CardTypeErr          = -2056, /**<Wrong card type                 0x83*/
        NDK_ERR_MI_ParaErrInIoctl       = -2057, /**<Wrong IOCTL parameter           0x82*/
        NDK_ERR_MI_Para                 = -2059, /**<Invalid parameter               0xa9*/

        NDK_ERR_WIFI_INVDATA            = -3001, /**<Invalid parameter*/
        NDK_ERR_WIFI_DEVICE_FAULT       = -3002, /**<Invalid device state*/
        NDK_ERR_WIFI_CMD_UNSUPPORTED    = -3003, /**<Command not supported*/
        NDK_ERR_WIFI_DEVICE_UNAVAILABLE = -3004, /**<Device unavailable*/
        NDK_ERR_WIFI_DEVICE_NOTOPEN     = -3005, /**<No AP scanned*/
        NDK_ERR_WIFI_DEVICE_BUSY        = -3006, /**<Device busy*/
        NDK_ERR_WIFI_UNKNOWN_ERROR      = -3007, /**<Unknown Error*/
        NDK_ERR_WIFI_PROCESS_INBADSTATE = -3008, /**<Failed to connect*/
        NDK_ERR_WIFI_SEARCH_FAULT       = -3009, /**<Invalied scanning state*/
        NDK_ERR_WIFI_DEVICE_TIMEOUT     = -3010, /**<Device timeout*/
        NDK_ERR_WIFI_NON_CONNECTED      = -3011, /**<Not connected*/

        NDK_ERR_RFID_BUSY               = -3101, /**<Rf card busy*/
        NDK_ERR_PRN_BUSY                = -3102, /**<Printer busy*/
        NDK_ERR_ICCARD_BUSY             = -3103, /**<Samer card busy*/
        NDK_ERR_MAG_BUSY                = -3104, /**<MagnetSmart card busy*/
        NDK_ERR_USB_BUSY                = -3105, /**<USB module busy*/
        NDK_ERR_WLM_BUSY                = -3106, /**<Wireless module busy*/
        NDK_ERR_PIN_BUSY                = -3107, /**<PIN input*/
        NDK_ERR_BT_BUSY                 = -3108, /**<Bluetooth module busy*/
        NDK_ERR_DEV_BUSY                = -3109, /**<Device busy*/
        NDK_ERR_BT_NOT_CONNECTED        = -3201, /**<Bluetooth not connected*/

        NDK_ERR_LINUX_ERRNO_BASE        = -5000, /**<Error prefix from system function*/
        NDK_ERR_LINUX_TCP_TIMEOUT  = (NDK_ERR_LINUX_ERRNO_BASE - 110), /**<Wrong TCP remote port*/
        NDK_ERR_LINUX_TCP_REFUSE   = (NDK_ERR_LINUX_ERRNO_BASE - 111), /**<TCP remote port not allowed*/
        NDK_ERR_LINUX_TCP_NOT_OPEN = (NDK_ERR_LINUX_ERRNO_BASE - 88),  /**<TCP not open*/
} EM_NDK_ERR;

/** @}*/ // End of type definitions and error codes

/** @addtogroup GUI
* @{
*/

#ifndef NL_COLOR_T
#define NL_COLOR_T
typedef unsigned int color_t; /**<RGB color value (0: Black; 0xFFFF: White)*/
#endif

/**
 *@brief Display modes
*/
typedef enum {
        TEXT_ATTRIBUTE_NORMAL      = 1 << 0, /**<Normal*/
        TEXT_ATTRIBUTE_REVERSE     = 1 << 1, /**<Reverse*/
        TEXT_ATTRIBUTE_UNDERLINE   = 1 << 2, /**<Underline*/
        TEXT_ATTRIBUTE_NOBACKCOLOR = 1 << 3, /**<No background color*/
} EM_TEXT_ATTRIBUTE;

/**
 *@brief Backlight control
*/
typedef enum {
        BACKLIGHT_OFF,    /**<On*/
        BACKLIGHT_ON,     /**<Off*/
        BACKLIGHT_LOCKON, /**<Always on*/
} EM_BACKLIGHT;

/**
 *@brief System font options
*/
typedef enum {
        DISPFONT_CUSTOM,      /**<Default size*/
        DISPFONT_EXTRA,       /**<Extra size*/
        DISPFONT_EXTRA_LARGE, /**<Extra large size*/
        DISPFONT_USER,        /**<Customized*/
} EM_DISPFONT;

/**
 *@brief TTF font encodings for display
*/
typedef enum {
	DISP_TTF_INPUT_ASCII   = 1, /**<ASCII*/
	DISP_TTF_INPUT_UNICODE = 2, /**<UNICODE*/
	DISP_TTF_INPUT_UTF8    = 3, /**<UTF8*/
}EM_DISPTTFINPUT;

/**
 *@brief System font color settings
*/
typedef enum {
        FONTCOLOR_NORMAL,     /**<Normal*/
        FONTCOLOR_REVERSE,    /**<Reverse*/
        FONTCOLOR_BG_REVERSE, /**<Reverse background color*/
} EM_FONTCOLOR;

/**
 *@brief Rectangular fill modes
*/
typedef	enum {
        RECT_PATTERNS_NO_FILL,   /**<Only draw borders*/
        RECT_PATTERNS_SOLID_FILL /**<Fill*/
} EM_RECT_PATTERNS;

/**
 *@brief	Status bar display control
*/
typedef	enum {
        STATUSBAR_DISP_CLOSE      = 0,       /**<Do not show status bar*/
        STATUSBAR_DISP_ALL        = 1,       /**<Show all(Time, Battery, Signal)*/
        STATUSBAR_DISP_TIME       = 2,       /**<Show time*/
        STATUSBAR_DISP_BATTERY    = 4,       /**<Show battery*/
        STATUSBAR_DISP_WLSIGNAL   = 8,       /**<Show wireless signal*/
        STATUSBAR_DISP_WIFI       = 0x10,    /**<Show WiFi signal*/
        STATUSBAR_DISP_MODEM      = 0x20,    /**<Show modem status*/
        STATUSBAR_DISP_ETH        = 0x40,    /**<Show ethernet status*/
        STATUSBAR_DISP_DATE       = 0x80,    /**<Show date*/
        STATUSBAR_DISP_BLUETOOTH  = 0x100,   /**<Show bluetooth status*/
        STATUSBAR_POSITION_TOP    = 0x00000, /**<Show status bar at the top*/
        STATUSBAR_POSITION_BOTTOM = 0x10000  /**<Show status bar at the bottom*/
} EM_STATUSBAR;

/**
 *@brief	GUI initialization
 *@details  Call the function first when application runs.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_INIT_CONFIG "NDK_ERR_INIT_CONFIG"                 Initialize configuration failed
 *@li	\ref NDK_ERR_CREAT_WIDGET "NDK_ERR_CREAT_WIDGET"                 Create interface failed
*/
int NDK_ScrInitGui(void);


/**
 *@brief	 Get display module version.
 *@param[out] pszVer	Module version (No less than 16 bytes)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		    Invalid parameter
*/
int NDK_ScrGetVer(char *pszVer);

/**
 *@brief	 Set display mode and get previous display mode.
 *@param[in]	emNewAttr	  New display mode
 *@param[out] pemOldAttr 	Previous display mode
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
*/
int NDK_ScrSetAttr(EM_TEXT_ATTRIBUTE emNewAttr, EM_TEXT_ATTRIBUTE *pemOldAttr);

/**
 *@brief 	Save current screen
 *@details	Save display content, cursor position and display mode, you can call \ref NDK_ScrPop "NDK_ScrPop" to restore display state rapidly.
			NDK_ScrPush and \ref NDK_ScrPop "NDK_ScrPop" are used in pairs, no nesting.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
*/
int NDK_ScrPush(void);

/**
 *@brief	 Pop current screen
 *@details 	Restore display state rapidly with what is saved by calling \ref NDK_ScrPush "NDK_ScrPush", the state includs display content, cursor position and display mode.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR "NDK_ERR"  				Fail
*/
int NDK_ScrPop(void);

/**
 *@brief	Set background image.
 *@details 	PNG, JPG, BMP format are supported.
 *@param[in]	pszFilePath 	Image file name
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PATH "NDK_ERR_PATH"  				Invalid path
 *@li	\ref NDK_ERR_DECODE_IMAGE "NDK_ERR_DECODE_IMAGE"  				Failed to decode image
*/
int NDK_ScrSetbgPic(char *pszFilePath);

/**
 *@brief		Clear background image.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_ScrClrbgPic(void);

/**
 *@brief		Clear screen
 *@details 	Move cursor to pixel coordinates (0,0), and set display mode as TEXT_ATTRIBUTE_NORMAL.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_ScrClrs(void);

/**
 *@brief	 Clear multiple pixel lines
 *details 	Move cursor to the coordinates (0, unStartY).
 *@param[in]	unStartY	Start line starting from 0
 *@param[in]	unEndY		End line starting from 0
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
*/
int NDK_ScrClrLine(uint unStartY, uint unEndY);

/**
 *@brief 	Move cursor
 *@details 	The cursor stays unmoved with illegal parameter.
 *@param[in]	unX 	X-coordinate
 *@param[in]	unY 	Y-coordinate
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
*/
int NDK_ScrGotoxy(uint unX, uint unY);

/**
 *@brief	 Get cursor position.
 *@param[out]	punX X-coordinate
 *@param[out]	punY Y-coordinate
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_ScrGetxy(uint *punX, uint *punY);

/**
 *@brief	 Set display area
 *@details  If not set, the display area is the actual screen size. Once display area  is set via this interface, display API are only effective in the area set. \n
			If you set (10,10,100,100) as the display area , the pixel coordinates (0,0) is
			(10,10) on screen, clearing screen can only remove data displayed within the area
 *@param[in]	unX		X-coordinate of display area
 *@param[in]	unY		Y-coordinate of display area
 *@param[in]	unWidth	Display area width
 *@param[in]	unHeight 	Display area height
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
*/
int NDK_ScrSetViewPort(uint unX, uint unY, uint unWidth, uint unHeight);

/**
 *@brief	 Get display area
 *@param[out]	punX		X-coordinate of display area
 *@param[out]	punY		Y-coordinate of display area
 *@param[out]	punWidth	Display area width
 *@param[out]	punHeight	Display area height
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_ScrGetViewPort(uint *punX, uint *punY, uint *punWidth, uint *punHeight);


/**
 *@brief	 Display bitmap image
 *@details   Bitmap image format: 8 pixel points in 1 byte, 0 for white point, 1 for black point, display data in horizontal arrangement as shown in the image below:\n
-----------------D7~~D0--------------D7~~D0------------------\n
Byte 1: ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð  ¡ð  ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð Byte2	\n
Byte 3: ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð  ¡ð  ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð Byte4	\n
Byte 5: ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð  ¡ð  ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð Byte6	\n
Byte 7: ¡ð ¡ð ¡ñ ¡ñ ¡ñ ¡ñ ¡ñ ¡ñ  ¡ñ  ¡ñ ¡ñ ¡ñ ¡ñ ¡ñ ¡ð ¡ð Byte8	\n
Byte 9: ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð  ¡ð  ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð Byte10	\n
Byte11: ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð  ¡ð  ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð Byte12	\n
Byte13: ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð  ¡ð  ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð Byte14	\n
Byte15: ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð  ¡ð  ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð Byte16	\n
---------------------------------------------------------------\n
	If the image to be displayed is outside of the screen or the area set via \ref NDK_ScrSetViewPort "NDK_ScrSetViewPort", this function will fail.
 *@param[in]	unX 		X-coordinate of upper-left corner of image
 *@param[in]	unY 		Y-coordinate of upper-left corner of image
 *@param[in]	unWidth 	Image width
 *@param[in]	unHeight 	Image height
 *@param[in]	psBuf 		Image data
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
*/
int NDK_ScrDrawBitmap(uint unX, uint unY, uint unWidth, uint unHeight, const char *psBuf);


/**
 *@brief	 Draw a line
 *@param[in]	unStartX 	X-coordinate of starting point
 *@param[in]	unStartY 	Y-coordinate of starting point
 *@param[in]	unEndX 		X-coordinate of end point
 *@param[in]	unEndY 		Y-coordinate of end point
 *@param[in]	unColor 		RGB color (0-0xFFFF)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_ScrLine(uint unStartX, uint unStartY, uint unEndX, uint unEndY, color_t unColor);


/**
 *@brief	 Draw a rectangle
 *@details	If the border of rectangle is outside of screen or the area set via \ref NDK_ScrSetViewPort "NDK_ScrSetViewPort", this function will fail.
 *@param[in]	unX 		X-coordinate of starting point of rectangle
 *@param[in]	unY 		Y-coordinate of starting point of rectangle
 *@param[in]	unWidth 		Rectangle width
 *@param[in]	unHeight 		Rectangle height
 *@param[in]	emFillPattern 	0: Not fill; 1: Fill
 *@param[in]	unColor	 RGB color (0-0xFFFF)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
*/
int NDK_ScrRectangle(uint unX, uint unY, uint unWidth, uint unHeight, EM_RECT_PATTERNS emFillPattern, color_t unColor);

/**
 *@brief	 Display image
 *@details	If the image displayed is outside of screen or the area set via \ref NDK_ScrSetViewPort "NDK_ScrSetViewPort", this function will fail.
 *@param[in]	unX 		X-coordinate of upper-left corner of image
 *@param[in]	unY 		Y-coordinate of upper-left corner of image
 *@param[in]	unWidth 	Image width
 *@param[in]	unHeight 	Image height
 *@param[in]	pszPic 	Image file name
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_DECODE_IMAGE "NDK_ERR_DECODE_IMAGE" 		 Failed to decode image
*/
int NDK_ScrDispPic(uint unX, uint unY, uint unWidth, uint unHeight, const char *pszPic);

/**
 *@brief	 Get pixel color
 *@param[in]	unX 		X-coordinate
 *@param[in]	unY 		Y-coordinate
 *@param[out]	punColor	RGB color value
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
*/
int NDK_ScrGetPixel(uint unX, uint unY, color_t *punColor);

/**
 *@brief	Draw pixel
 *@param[in]	unX 		X-coordinate
 *@param[in]	unY		Y-coordinate
 *@param[in]	unColor 	RGB color value (0-0xFFFF)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"   Invalid parameter
*/
int NDK_ScrPutPixel(uint unX, uint unY, color_t unColor);

/**
 *@brief Refresh display
 *@details	Auto refresh is enabled by default. To avoid blinking screen, you can disable auto refresh via \ref NDK_ScrAutoUpdate "NDK_ScrAutoUpdate", LCD display will not be refreshed until \ref NDK_ScrRefresh "NDK_ScrRefresh" is called.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_ScrRefresh(void);

/**
 *@brief	 Refresh full screen
 *@details	This interface differs from \ref NDK_ScrRefresh "NDK_ScrRefresh" in that it is not limited by the size of display area, when setting display area in full screen, \ref NDK_ScrRefresh "NDK_ScrRefresh" will have the same effect.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_ScrFullRefresh(void);

/**
 *@brief	 Enable/Disable auto refresh.
 *@param[in] nNewAuto 0: Disable auto refresh; Non-zero: Enable auto refresh
 *@param[out]	pnOldAuto	Previous auto refresh setting.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_ScrAutoUpdate(int nNewAuto, int *pnOldAuto);

/**
 *@brief	 Get LCD size
 *@param[out]		punWidth	LCD width.
 *@param[out]		punHeight	LCD height.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA"   Invalid parameter
*/
int NDK_ScrGetLcdSize(uint *punWidth, uint *punHeight);

/**
 *@brief	 Get LCD color depth
 *@details	It can be used to check monochrome or color LCD
 *@param[out] 	punColorDepth	1: Monochrome; 16: 16-bit color
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA"   Invalid parameter
*/
int NDK_ScrGetColorDepth(uint *punColorDepth);

/**
 *@brief Control LCD backlight.
 *@param[in]	emBL	BACKLIGHT_OFF: Turn off backlight\n
 					BACKLIGHT_ON: Turn on backlight\n
 					BACKLIGHT_LOCKON: Backlight always on
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	      \ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV"  Failed to open device
 *@li	      \ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		Failed to call driver function
*/
int NDK_ScrBackLight(EM_BACKLIGHT emBL);

/**
 *@brief		Set screen contrast.
 *@param[in]		unContrast	Contrast level (0-7, 0 is the most weak, default is 5)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	      \ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV"  Failed to open device
 *@li	      \ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		Failed to call driver function
*/
int NDK_ScrSetContrast(uint unContrast);

/**
 *@brief	 Set display font.
 *@details  Allows for switching between two sizes of bitmap fonts by default, different models may have different default font size\n
			Color display supports: 8x16 (DISPFONT_CUSTOM); 24x24, 12x24 (DISPFONT_EXTRA)\n
			Monochrome display support: 6x12 (DISPFONT_CUSTOM); 8x16 (DISPFONT_EXTRA)\n
			New font is only effective after this function is called
 *@param[in]	emType	Display font
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	      \ref NDK_ERR "NDK_ERR" 		Fail
*/
int NDK_ScrSetFontType(EM_DISPFONT emType);

/**
 *@brief    Get display font.
 *@param[out]    pemType  Current display font
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
*/
int NDK_ScrGetFontType(EM_DISPFONT *pemType);

/**
 *@brief	 Get font width and height of current font.
 *@param[out] 	punWidth	Width of character.
 *@param[out] 	punHeight	Height of character.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_ScrGetFontSize(uint *punWidth, uint *punHeight);

/**
 *@brief	 Add font
 *@details   After adding custom font, call \ref NDK_ScrSetFontType "NDK_ScrSetFontType" with DISPFONT_USER to use it.
 *@param[in]	psCnPath	 Chinese character font file path
 *@param[in]	psEnPath	 ASCII character font file path
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	      \ref NDK_ERR "NDK_ERR" 		Fail
*/
int NDK_ScrFontAdd(const char *psCnPath, const char *psEnPath);

/**
 *@brief	Set line spacing and word spacing.
 *@details  Set unWspace 2 times of the width of the pixel
 *@param[in]	unWspace	Word spacing
 *@param[in]	unHspace	    Line spacing
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail

*/
int NDK_ScrSetSpace(uint unWspace, uint unHspace);

/**
 *@brief	 Set font color
 *@param[in]	unColor	 RGB color value
 *@param[in]	emType	Font to set

 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
*/
int NDK_ScrSetFontColor(color_t unColor, EM_FONTCOLOR emType);

/**
 *@brief	 Display string.
 *@param[in]	unX	X-coordinate of string
 *@param[in]	unY	 Y-coordinate of string
 *@param[in]	pszS	String to be displayed
 *@param[in]	unMode	 1: Small font (Monochrome display: 8x8; Color display: 8x16)\n
			0: Current font
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
  *@li	    \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
*/
int NDK_ScrDispString(uint unX, uint unY, const char *pszS, uint unMode);

/**
 *@brief	 Formatted output on screen (Similar to printf)
 *@param[in]	psFormat	Format string
  *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
*/
int NDK_ScrPrintf(const char *psFormat, ...);

/**
 *@brief	Control status bar
 *@details	Status bar is not shown by default
 *@param[in]	emFlag	0: Do not show the status bar\n
					1: Show all including signal, time and battery\n
					2: Show time only\n
					4: Show battery only\n
					8: Show one wireless signal only
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li      	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter

*/
int NDK_ScrStatusbar(EM_STATUSBAR emFlag);

/**
 *@brief	Load BDF font
 *@param[in]	  unFontId A custom font ID starting from 0. If uFontID BDF font is the same as used before, the previous one will be overrided
 *@param[in]    pszFile BDF file path and name
 *@param[in]    punWidth Width in pixel
 *@param[in]   punHeight Height in pixel
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA"  				Invalid parameter
 *@li	      \ref NDK_ERR "NDK_ERR"  				BDF Font not loaded
*/
int NDK_ScrLoadBDF(uint unFontId, char *pszFile, uint *punWidth, uint *punHeight);

/**
 *@brief	Display BDF font text
 *@param[in]	unFontId Font ID
 *@param[in]	unX	X-Coordinate
 *@param[in]	unY	Y-Coordinare
 *@param[in]   pusText Text string
 *@return
 *@li	\ref NDK_OK "NDK_OK"  	Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA"  				Invalid parameter
 *@li	      \ref NDK_ERR "NDK_ERR"  				BDF Font not loaded
*/
int NDK_ScrDispBDFText(uint unFontId, uint unX, uint unY, ushort *pusText);

/**
 *@brief    Calibrate touch screen
 *@return
 *@li	\ref NDK_OK "NDK_OK"    Success
 *@li	\ref NDK_ERR_NO_DEVICES     "NDK_ERR_NO_DEVICES"        Not touchscreen device
 *@li	\ref NDK_ERR_QUIT           "NDK_ERR_QUIT"              Canceled
 *@li	\ref NDK_ERR                "NDK_ERR"                   Fail
*/
int  NDK_ScrTSCalibrate(void);

/**
 *@brief    Decode image
 *@param[in]    pszFile		Image file name (PNG, JPG, BMP format supported)
 *@param[out]   pOutput 	Buffer to store image data
 *@return
 *@li	\ref NDK_OK "NDK_OK"    Success
 *@li	\ref NDK_ERR_PARA     "NDK_ERR_PARA"         Invalid parameter
 *@li	\ref NDK_ERR_PATH	  "NDK_ERR_PATH"   Image file not exist
 *@li		    \ref NDK_ERR	"NDK_ERR"  Fail
*/
int  NDK_ScrImgDecode(char *pszFile, char **pOutput);

/**
 *@brief    Free image data buffer.
 *@details  Used in pairs with NDK_ScrImgDecode "NDK_ScrImgDecode"
 *@param[in]    pImg	Image data buffer to be freed
 *@return
 *@li	\ref NDK_OK "NDK_OK"    Success
 *@li	\ref NDK_ERR_PARA     "NDK_ERR_PARA"        Invalid parameter
*/
int NDK_ScrImgDestroy(char *pImg);

/**
 *@brief    Display image using image data buffer
 *@param[in]     unX  X-Coordinare of top left corner of display area
 *@param[in]     unY  Y-Coordinare of top left corner of display area
 *@param[in]     unW  Width of display area
 *@param[in]     unH  Height of display area
 *@param[in]     pImg Image data buffer
 *@return
 *@li	\ref NDK_OK "NDK_OK"    Success
 *@li	\ref NDK_ERR_PARA     "NDK_ERR_PARA"       Invalid parameter
*/
int NDK_ScrDispImg(uint unX, uint unY, uint unW, uint unH, char *pImg);

/**
 *@brief    Get font data buffer
 *@param[in]    pszText  String text
 *@param[out]    psOutbuf  Data in RGB565 format
 *@param[in]    unBuf_x  X-coordinate of upper left corner of the starting point
 *@param[in]    unBuf_y  Y-Coordinate of upper left corner of the starting point
 *@param[in]    unBuf_w psOutbuf  Pixel width
 *@param[in]    unBuf_h psOutbuf  Pixel height
 *@param[in]    usColor Font display color, rgb565 format, 0 for black, 0xFFFF for white

 *@return
 *@li	\ref NDK_OK "NDK_OK"    Success
 *@li	\ref NDK_ERR_PARA     "NDK_ERR_PARA"        Invalid parameter
 *@li	\ref NDK_ERR_OVERFLOW  "NDK_ERR_OVERFLOW"  Buffer too short
*/
int NDK_ScrGetFontBuf(const char *pszText, unsigned short* psOutbuf, unsigned int unBuf_x, unsigned int unBuf_y, unsigned int unBuf_w, unsigned int unBuf_h, unsigned short usColor);

/**
 *@brief	  Load TTF font
 *@param[in]	  filepathname TTF font file path name
 *@param[in]	  face_index Face index in font file, set 0 if only one
 *@return
 *@li	\ref NDK_OK "NDK_OK"    Success
 *@li	\ref NDK_ERR_PARA     "NDK_ERR_PARA"        Invalid parameter
 *@li	\ref NDK_ERR                "NDK_ERR"       Invalid file format
*/
int NDK_ScrInitTrueTypeFont(const char* filepathname, int face_index);

/**
 *@brief    Set width and height of TTF font
 *@param[in]    width    Font width
 *@param[in]    height   Font height
 *@return
 *@li	\ref NDK_OK "NDK_OK"    Success
 *@li	\ref NDK_ERR	"NDK_ERR"  Fail
*/
int NDK_SrcSetTrueTypeFontSizeInPixel(int width, int height);

/**
 *@brief    Get width and height of TTF font
 *@param[in]    str    Display string
 *@param[in]    stringLength    Display string length
 *@param[out]    width    Font width
 *@param[out]    height   Font height
 *@return
 *@li	\ref NDK_OK "NDK_OK"    Success
 *@li	\ref NDK_ERR	"NDK_ERR"  Fail
*/
int NDK_SrcGetTrueTypeFontSize(const char *str, int stringLength, int *width, int *height);

/**
 *@brief    Set encoding format
 *@param[in]    input \ref EM_DISPTTFINPUT "EM_DISPTTFINPUT"
 *@return
 *@li	\ref NDK_OK "NDK_OK"    Success
*/
int NDK_ScrSetTrueTypeFontEncodingFormat(EM_DISPTTFINPUT input);

/**
 *@brief	 Display string with TTF font
 *@param[in]	unX	X-coordinate of string
 *@param[in]	unY	 Y-coordinate of string
 *@param[in]	str	String to be displayed in ASCII or UNICODE format
 *@param[in]	stringLength String length to be displayed
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR                "NDK_ERR"       Invalid file format

*/
int NDK_ScrDispTrueTypeFontText(uint unX, uint unY, const char *str, int stringLength);

/**
 *@brief    Destroy TTF font
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
*/
int NDK_ScrDestroyTrueTypeFont(void);

/** @}*/ // End of display

/** @addtogroup Input
* @{
*/

/**
 *@brief Function keys
*/
#define  K_F1        0x01
#define  K_F2        0x02
#define  K_F3        0x03
#define  K_F4        0x04
#define  K_F5        0x05
#define  K_F6        0x06
#define	 K_F7        0x07
#define	 K_F8        0x08
#define	 K_F9        0x09
#define  K_BASP      0x0a /**<Backspace*/
#define  K_ENTER     0x0D /**<Cancel*/
#define  K_ESC       0x1B /**<Enter*/
#define	 K_ZMK       0x1C /**<Letter*/
#define	 K_DOT       0x2E /**<Decimal point*/

/**
 *@brief Number keys
*/
#define K_ZERO       0x30
#define K_ONE        0x31
#define K_TWO        0x32
#define K_THREE      0x33
#define K_FOUR       0x34
#define K_FIVE       0x35
#define K_SIX        0x36
#define K_SEVEN      0x37
#define K_EIGHT      0x38
#define K_NINE       0x39

/**
 *@brief Input control
*/
typedef enum {
        INPUT_CONTRL_NOLIMIT,         /**<Any ASCII characters allowed*/
        INPUT_CONTRL_LIMIT,           /**<Only numbers and decimal point allowed*/
        INPUT_CONTRL_NOLIMIT_ERETURN, /**<Any ASCII characters allowed, waiting for Enter key*/
        INPUT_CONTRL_LIMIT_ERETURN,   /**<Only numbers and decimal point allowed, waiting for Enter key*/
} EM_INPUT_CONTRL;

/**
 *@brief Display control
*/
typedef enum {
        INPUTDISP_NORMAL, /**<Display character*/
        INPUTDISP_PASSWD, /**<Display '*'*/
        INPUTDISP_OTHER,
} EM_INPUTDISP;

/**
 *@brief Input method types
*/
typedef enum {
        IME_ENGLISH   = 1,
        IME_NUM       = 2,
        IME_HANDWRITE = 6,
        IME_MAXNUM = IME_HANDWRITE,
} EM_IME;

/**
 *@brief Touch screen coordinates status
*/
typedef enum {
        PADSTATE_UP,   /**<Touch point released*/
        PADSTATE_DOWN, /**<Touch point pressed*/
        PADSTATE_KEY,  /**<Key pressed*/
} EM_PADSTATE;

/**
 *@brief Touch screen information
*/
typedef struct __st_PADDATA {
        uint unPadX;            /**<X-axis*/
        uint unPadY;            /**<Y-axis*/
        uint unKeycode;         /**<Key code*/
        EM_PADSTATE emPadState; /**<Coordinate status*/
} ST_PADDATA;

/**
 *@brief	Clear keypad buffer
 *@return
 *@li	\ref NDK_OK "NDK_OK"    Success
*/
int NDK_KbFlush(void);

/**
 *@brief	Enable/Disable hold-on key and combination key support.
 *@param[in] nSelect 	0: Disable; 1: Enable
 *@param[in] nMode 		0: Hold-on key;  1:Combination key
 *@param[in] pnState 	Previous state of hold-on key or combination key support (0: Disable; 1: Enable)
 *@return
 *@li   \ref NDK_OK "NDK_OK"    Success
 *@li	  \ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_KbSwitch(int nSelect, int nMode, int *pnState);

/**
 *@brief 	Get hold-on key or key combination support.
 *@param[in] nMode 		0: Hold-on key;  1:Combination key
 *@param[out] pnState 	Current state of hold-on key or combination key support (0: Disable; 1: Enable)
 *@return
 *@li   \ref NDK_OK "NDK_OK"    Success
 *@li	  \ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_KbGetSwitch(int nMode, int *pnState);

/**
 *@brief 	Read key
 *@details	The process is as follows: press one key, release it, and return key code
 *@param[in] unTime	Timeout in seconds (0: Keep on waiting for reading key)
 *@param[out] pnCode	Key code. (0: No key pressed)
 *@return
 *@li     \ref NDK_OK "NDK_OK"    Success
 *@li	  \ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_KbGetCode(uint unTime, int *pnCode);

/**
 *@brief	 Read key (Non-block)
 *@details	Check if there is a keystroke in keystroke buffer, if yes, read the key and return key code, if not, return 0 immediately.
   			Generally, it is used in a loop, and \ref NDK_KbFlush "NDK_KbFlush" shall be used to clear buffer before.
 			This function differs from \ref NDK_KbGetCode "NDK_KbGetCode" in that it will return immediately without waiting.
 *@param[out] pnCode	Key code. Return 0 if no key is pressed.
 *@return
 *@li     \ref NDK_OK "NDK_OK"    Success
 *@li	\ref NDK_ERR_PARA     "NDK_ERR_PARA"       Invalid parameter
*/
int NDK_KbHit(int *pnCode);

/**
*@brief		Input string
*@details	Read from keyboard a string ended with a line feed, and save it in buffer pszBuf.
			For ESC key, it will return Fail, for Enter key, it will finish reading key and return, other function keys are invalid.
*@param[out] 	pszBuf	Data buffer saving the string
*@param[in] 	unMin	Minimum length of input string
*@param[in] 	unMaxLen	Maximum length of input string
*@param[out] 	punLen	Actual length of input length
*@param[in] 	emMode	INPUTDISP_NORMAL: Display characters when the value\n
					INPUTDISP_PASSWD: Display '*'\n
					INPUTDISP_OTHER: Displayed in plain code.
*@param[in] 	unWaitTime	Timeout in seconds. (0: Block)
*@param[in] 	emControl	INPUT_CONTRL_NOLIMIT:Any ASCII characters\n
						INPUT_CONTRL_LIMIT:Only Numbers and the decimal point\n
						INPUT_CONTRL_NOLIMIT_ERETURN: Any ASCII code character, wait for Enter key to return\n
						INPUT_CONTRL_LIMIT_ERETURN:Read only numbers and decimal points, wait for Enter key to return
 *@return
 *@li	\ref NDK_OK "NDK_OK"    Success
 *@li   		\ref NDK_ERR_PARA "NDK_ERR_PARA" 	  Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR"             Fail
*/
int NDK_KbGetInput(char *pszBuf, uint unMin, uint unMaxLen, uint *punLen, EM_INPUTDISP emMode, uint unWaitTime, EM_INPUT_CONTRL emControl);

/**
 *@brief	Get touch point
  *@details	The API can get both touch point coordinate and key value.\n
             NOTE:It can be affected when called  \ref NDK_KbHit "NDK_KbHit()" or \ref NDK_KbGetCode "NDK_KbGetCode" function after it. Return value "PADSTATE_KEY" means that key is pressed and key value is stored in "pstPaddata->unKeycode"
 *@param[out] pstPaddata	Touch point coordinate(\ref ST_PADDATA "ST_PADDATA")
 *@param[in] unTimeOut	0: Block; Non-zero: Timeout in milliseconds
 *@return
 *@li   \ref NDK_OK "NDK_OK"  				Success
 *@li   	\ref NDK_ERR_PARA     "NDK_ERR_PARA"	  Invalid parameter
 *@li   	\ref NDK_ERR_TIMEOUT  "NDK_ERR_TIMEOUT"	  timeout error
*/
int NDK_KbGetPad(ST_PADDATA *pstPaddata, uint unTimeOut);

/** @}*/ // End of keypad

/** @addtogroup Printer
* @{
*/

/**
 *@brief Fonts
*/
typedef enum {
        PRN_HZ_FONT_24x24 = 1,
        PRN_HZ_FONT_16x32,
        PRN_HZ_FONT_32x32,
        PRN_HZ_FONT_32x16,
        PRN_HZ_FONT_24x32,
        PRN_HZ_FONT_16x16,
        PRN_HZ_FONT_12x16,
        PRN_HZ_FONT_16x8,
        PRN_HZ_FONT_24x24A,
        PRN_HZ_FONT_24x24B,
        PRN_HZ_FONT_24x24C,
        PRN_HZ_FONT_24x24USER,
        PRN_HZ_FONT_12x12A,
        PRN_HZ_FONT_48x24A,
        PRN_HZ_FONT_48x24B,
        PRN_HZ_FONT_48x24C,
        PRN_HZ_FONT_24x48A,
        PRN_HZ_FONT_24x48B,
        PRN_HZ_FONT_24x48C,
        PRN_HZ_FONT_48x48A,
        PRN_HZ_FONT_48x48B,
        PRN_HZ_FONT_48x48C
} EM_PRN_HZ_FONT;

typedef enum {
        PRN_ZM_FONT_8x16 = 1,
        PRN_ZM_FONT_16x16,
        PRN_ZM_FONT_16x32,
        PRN_ZM_FONT_24x32,
        PRN_ZM_FONT_6x8,
        PRN_ZM_FONT_8x8,
        PRN_ZM_FONT_5x7,
        PRN_ZM_FONT_5x16,
        PRN_ZM_FONT_10x16,
        PRN_ZM_FONT_10x8,
        PRN_ZM_FONT_12x16A, /**<MSGothic bold 12x16 bitmap*/
        PRN_ZM_FONT_12x24A, /**<Gulimche font 12x24 bitmap*/
        PRN_ZM_FONT_16x32A, /**<MSGothic bold 16x32 bitmap*/
        PRN_ZM_FONT_12x16B, /**<MSGothic bold 12x16 bitmap*/
        PRN_ZM_FONT_12x24B, /**<MSGothic bold 12x24 bitmap*/
        PRN_ZM_FONT_16x32B, /**<MSGothic bold 16x32 bitmap*/
        PRN_ZM_FONT_12x16C,
        PRN_ZM_FONT_12x24C,
        PRN_ZM_FONT_16x32C,
        PRN_ZM_FONT_24x24A,
        PRN_ZM_FONT_32x32A,
        PRN_ZM_FONT_24x24B,
        PRN_ZM_FONT_32x32B,
        PRN_ZM_FONT_24x24C,
        PRN_ZM_FONT_32x32C,
        PRN_ZM_FONT_12x12,
        PRN_ZM_FONT_8x32
} EM_PRN_ZM_FONT;

typedef enum {
        PRN_GETFONT_16X16 = 1,
        PRN_GETFONT_MAX
} EM_PRN_GETFONT;

/**
 *@brief  Printer status
*/
typedef enum {
        PRN_STATUS_OK       = 0,  /**<Good*/
        PRN_STATUS_BUSY     = 8,  /**<In printing*/
        PRN_STATUS_NOPAPER  = 2,  /**<Out of paper*/
        PRN_STATUS_OVERHEAT = 4,  /**<Overheat*/
        PRN_STATUS_VOLERR   = 112 /**<Abnormal voltage*/
} EM_PRN_STATUS;

/**
 *@brief  AD channel types
*/
typedef enum {
        PRN_CHANELNUM_VOLTAGE,    /**<Voltage sampling*/
        PRN_CHANELNUM_TEMPERATURE /**<Temperature sampling*/
} EM_PRN_CHANELNUM;

/**
 *@brief  Printer types
*/
typedef enum {
        PRN_TYPE_TP = 0, /**<Thermal*/
        PRN_TYPE_HIP,    /**<Punch*/
        PRN_TYPE_FIP,    /**<Friction*/
        PRN_TYPE_END
} EM_PRN_TYPE;

/**
 *@brief  Printing modes
*/
typedef enum {
        PRN_MODE_ALL_DOUBLE = 0, /**<Enlarge horizontally and vertically*/
        PRN_MODE_WIDTH_DOUBLE,   /**<Enlarge horizontally*/
        PRN_MODE_HEIGHT_DOUBLE,  /**<Enlarge vertically*/
        PRN_MODE_NORMAL          /**<No enlargement*/
} EM_PRN_MODE;


/**
 *@brief Underline setting
*/
typedef enum {
        PRN_UNDERLINE_STATUS_OPEN = 0, /**<Enable underline*/
        PRN_UNDERLINE_STATUS_CLOSE     /**<Disable underline*/
} EM_PRN_UNDERLINE_STATUS;


/**
 *@brief TTF font encodings for print
*/
typedef enum {
	PRN_TTF_INPUT_ASCII   = 1, /**<ASCII*/
	PRN_TTF_INPUT_UNICODE = 2, /**<UNICODE*/
	PRN_TTF_INPUT_UTF8    = 3, /**<UTF8*/
}EM_PRNTTFINPUT;

/**
 *@brief Font data
*/
typedef struct {
        int nOffset;   /**<Offset in font file*/
        int nFontByte; /**<Bytes in font file*/
} ST_PRN_RECMSG;


/**
 *@brief Font information
*/
typedef struct {
        char *pszName;                         /**<Font file name or its path*/
        int  nNum;                             /**<Register No. 0-7.*/
        int  nDirection;                       /**<0: Horizontal font; 1: Vertical font*/
        int  w;                                /**<Width of font*/
        int  h;                                /**<Height of font*/
        int  (*func)(char *, ST_PRN_RECMSG *); /**<Function to get offset and number of bytes*/
        int  nIsHZ;                            /**<Always use 0*/
} ST_PRN_FONTMSG;

/*
*@brief Feed paper setting
*/
typedef enum {
        PRN_FEEDPAPER_BEFORE = 0,         /**<Unfold paper before printing a single page*/
        PRN_FEEDPAPER_AFTER             /**<Feed paper after printing a single page*/
} EM_PRN_FEEDPAPER;

/**
 *@brief 		  Initialize printer
 *@details  	Clear buffer, reset print parameters (incl. font, margin, mode etc.)
 *@param[in]      unPrnDirSwitch  Enable/Disable sending data whiling printing.\n
              0: Disable (Default), start all printing only after \ref NDK_PrnStart "NDK_PrnStart" is called.\n
              1: Enable, once there is a full line of data, it will be sent to print immediately, calling \ref NDK_PrnFeedByPixel "NDK_PrnFeedByPixel" will start paper feed and return immediately.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  					Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
 *@li	      \ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open printer device
 *@li	      \ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		         Failed to call driver function
 *@li	      \ref NDK_ERR_MACLLOC "NDK_ERR_MACLLOC" 		Out of memory
*/
int NDK_PrnInit(uint unPrnDirSwitch);

/**
 *@brief 		Print string
 *@details 		This function will convert all strings to be printed to matrix buffer, sending data and printing will begin after Start is called.
 *@param[in] 	pszBuf String ended with '\0' with content of ASCII or line feed
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	      \ref NDK_ERR_INIT_CONFIG "NDK_ERR_INIT_CONFIG" 		Failed to initialize configuration
 *@li	      \ref NDK_ERR_MACLLOC "NDK_ERR_MACLLOC" 		 Out of memory
 *@li	\ref EM_PRN_STATUS   "EM_PRN_STATUS"    Printet status
*/
int NDK_PrnStr(const char *pszBuf);

/**
 *@brief 		Get A/D sample value (Not implemented).
 *@param[in] emChanelNum    	Channel number (\ref EM_PRN_CHANELNUM "EM_PRN_CHANELNUM")
 *@param[out] 	punAdValue     	Sample value (Voltage in V units, Temperature in 0.1C units)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_PrnGetAd(EM_PRN_CHANELNUM emChanelNum, uint *punAdValue);

/**
 *@brief 	Start printing.
 *@details 	Both \ref NDK_PrnStr "NDK_PrnStr" and \ref NDK_PrnImage "NDK_PrnImage" will complete data-to-dot matrix conversion and send it to the buffer, call these functions to start sending data and printing. \ref NDK_PrnStart "NDK_PrnStart" will stop and return printer state after printing is completed. Applications may use the returned value to check if the printer is in the right state.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				End of printing and the printer is in good state
 *@li	    \ref NDK_ERR_INIT_CONFIG "NDK_ERR_INIT_CONFIG" 		Failed to initialize configuration
 *@li	      \ref NDK_ERR_MACLLOC "NDK_ERR_MACLLOC" 		  Out of memory
 *@li	      \ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref EM_PRN_STATUS   "EM_PRN_STATUS"     Printer status
*/
int NDK_PrnStart(void);

/**
 *@brief 		Print image
 *@details 	This function shall also convert bit matrix to be printed to the buffer and call \ref NDK_PrnStart "NDK_PrnStart" to start printing. Maximum width of thermal printer is 384 pixels. If the sum of xsize and xpos surpasses the limit above, the function will fail. As for horizontal enlargement mode, the value shall not exceed 384/2.
 *@param[in] 		unXsize Image width in pixel
 *@param[in] 		unYsize Image height in pixel
 *@param[in] 		unXpos Column of the top left of image and must meet xpos + xsize < = ndk_PR_MAXLINEWIDE (ndk_PR_MAXLINEWIDE is 384 for normal mode, 384/2 for horizontal enlargement)
 *@param[in]		psImgBuf  Dot matrix data for the image in horizontal, the first 8 dot of line 1 is byte 1, D7 is the first dot
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
*/
int NDK_PrnImage(uint unXsize, uint unYsize, uint unXpos, const char *psImgBuf);

/**
 *@brief 		Get printer driver version
 *@param[out]  pszVer Buffer to store the version string
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	      \ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	Failed to open printer device
 *@li	      \ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		Failed to call driver function
*/
int NDK_PrnGetVersion(char *pszVer);

/**
 *@brief 		Set print font
 *@param[in] 	emHZFont	Chinese font
 *@param[in]  emZMFont	ASCII font
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_PrnSetFont(EM_PRN_HZ_FONT emHZFont, EM_PRN_ZM_FONT emZMFont);

/**
 *@brief		Get printer status
 *@details		This function can be used to check if printer is out of paper before printing starts.
 *@param[out]   pemStatus Printer status returned
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	      \ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	Failed to open printer device
 *@li	       \ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		Failed to call driver function
*/
int NDK_PrnGetStatus(EM_PRN_STATUS *pemStatus);

/**
 *@brief 	Set print mode.
 *@param[in] 	emMode Print mode (Normal mode by default)
 *@param[in]  unSigOrDou 0: Unidirectional; 1: Bidirectional (For dot matrix printer only)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
*/
int NDK_PrnSetMode(EM_PRN_MODE emMode, uint unSigOrDou);

/**
 *@brief		Set print grayscale (For thermal printer only)
 *@param[in]    unGrey Grey value: 0-5. (0: Lightest; 5: Darkest)\n Default: 3
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail

*/
int NDK_PrnSetGreyScale(uint unGrey);

/**
 *@brief  	Set left margin, word spacing and line spacing for printing.
 *@details 	The effectiveness of setting on printer will maintain until next time.
 *@param[in]    unBorder Left margin: 0-288 (Default: 0)
 *@param[in]    unColumn Word spacing: 0-255(Default: 0)
 *@param[in]    unRow Line spacing: 0-255 (Default: 0)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameters
 *@li	      \ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open printer device
 *@li	      \ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 	 Failed to call driver function

*/
int NDK_PrnSetForm(uint unBorder, uint unColumn, uint unRow);

/**
 *@brief 	  Feed paper by pixel.
 *@details	Paper will not be fed immediately after the function is called. It will be saved in buffer and wait until NDK_PrnStart is called.
 *@param[in]   unPixel  Pixels to feed
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	      \ref NDK_ERR_INIT_CONFIG "NDK_ERR_INIT_CONFIG" 	Failed to initialize configuration
 *@li	      \ref NDK_ERR_MACLLOC "NDK_ERR_MACLLOC" 		 Out of memory
*/
int NDK_PrnFeedByPixel(uint unPixel);

/**
 *@brief	 Enable/Disable underline.
 *@param[in]  emStatus 0: Enable underline; 1: Disable underline
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
*/
int NDK_PrnSetUnderLine(EM_PRN_UNDERLINE_STATUS emStatus);

/**
 *@brief	Register custom font(For thermal printer only).
 *@param[in]    pstMsg   Pointer of \ref ST_PRN_FONTMSG "ST_PRN_FONTMSG"
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_PrnFontRegister(ST_PRN_FONTMSG *pstMsg);

/**
 *@brief	 Select print font
 *@param[in]  unFontId  Registered font ID(Will override font set in \ref NDK_PrnSetFont "NDK_PrnSetFont")
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_PrnSetUsrFont(uint unFontId);

/**
 *@brief	 Print BMP, PNG and JPG images
 *@details Image decoding takes time and may cause a certain length of latency if necessary.
 *@param[in] pszPath    Image file name
 *@param[in] unXpos	    Coloum position of upper-left corner of image
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	      \ref NDK_ERR_INIT_CONFIG "NDK_ERR_INIT_CONFIG" 		Failed to initialize configuration
 *@li	      \ref NDK_ERR_DECODE_IMAGE "NDK_ERR_DECODE_IMAGE" 	  Failed to decode image
 *@li	      \ref NDK_ERR_MACLLOC "NDK_ERR_MACLLOC"               Out of memory
*/
int NDK_PrnPicture(uint unXpos, const char *pszPath);

/**
 *@brief	Set page length (Not valid for thermal printer)
 *@param[in]  unLen	Page length: 0-792
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	      \ref NDK_ERR_PARA "NDK_ERR_PARA"   Invalid parameter
 *@li	      \ref NDK_ERR_NOT_SUPPORT "NDK_ERR_NOT_SUPPORT"        Not supported

*/
int NDK_PrnSetPageLen(uint unLen);

/**
 *@brief	Load BDF Font for print
 *@details  It may take some time if font file is big.
 *@param[in]  pszPath BDF font file name
 *@return
 *@li \ref NDK_OK "NDK_OK"  			Success
 *@li	\ref NDK_ERR_NOT_SUPPORT "NDK_ERR_NOT_SUPPORT"      Not supported
 *@li	\ref NDK_ERR "NDK_ERR"       Fail
 *@li	\ref NDK_ERR_MACLLOC "NDK_ERR_MACLLOC"       Out of memory
*/
int NDK_PrnLoadBDFFont(const char *pszPath);

/**
 *@brief	Print BDF text string
 *@param[in]  pusText Text string
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_NOT_SUPPORT "NDK_ERR_NOT_SUPPORT"       Not supported

*/
int NDK_PrnBDFStr(ushort *pusText);

/**
 *@brief	Set BDF font attributes
 *@param[in]  unXpos  Left margin: 0-288 (Default: 0)
 *@param[in]  unLineSpace  Line space: 0-255 (Default: 0)
 *@param[in]  unWordSpace  Word space: 0-255 (Default: 0)
 *@param[in]  unXmode  Horizontal enlargement (MaxWidth * unXmode must not more than 384)
 *@param[in]  unYmode  Vertical enlargement (MaxHeight * unYmode must not more than 48)
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
 *@li	\ref NDK_ERR_NOT_SUPPORT "NDK_ERR_NOT_SUPPORT"     Not supported
*/
int NDK_PrnSetBDF(uint unXpos, uint unLineSpace, uint unWordSpace, uint unXmode, uint unYmode);

/**
 *@brief  Feed paper
 *@param[in]  emType		\ref EM_PRN_FEEDPAPER "EM_PRN_FEEDPAPER"
 *@return
 *@li   \ref NDK_OK "NDK_OK"                          Success
 *@li \ref NDK_ERR_PARA "NDK_ERR_PARA"      Invalid parameter
 *@li \ref NDK_ERR_INIT_CONFIG "NDK_ERR_INIT_CONFIG"     Failed to initialize configuration
 *@li \ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV"       Failed to open print device
 *@li \ref NDK_ERR_IOCTL "NDK_ERR_IOCTL"        Failed to call driver function
 *@li	\ref NDK_ERR_NOT_SUPPORT "NDK_ERR_NOT_SUPPORT"       Not supported
*/
int NDK_PrnFeedPaper(EM_PRN_FEEDPAPER emType);

/**
 *@brief    Get font data
 *@param[in]    emGetFont Font size (Only 16x16 supported)
 *@param[in]    pszBuf Input string
 *@param[out]   psOutbuf   Horizontal matrix data, as shown in the following figure:
-----------------D7~~D0--------------D7~~D0------------------\n
Byte 1: ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð  ¡ð  ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð Byte2 \n
Byte 3: ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð  ¡ð  ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð Byte4 \n
Byte 5: ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð  ¡ð  ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð Byte6 \n
Byte 7: ¡ð ¡ð ¡ñ ¡ñ ¡ñ ¡ñ ¡ñ ¡ñ  ¡ñ  ¡ñ ¡ñ ¡ñ ¡ñ ¡ñ ¡ð ¡ð Byte8 \n
Byte 9: ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð  ¡ð  ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð Byte10    \n
Byte11: ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð  ¡ð  ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð Byte12    \n
Byte13: ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð  ¡ð  ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð Byte14    \n
Byte15: ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð  ¡ð  ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð ¡ð Byte16    \n
---------------------------------------------------------------\n
 *@param[in]    unBuf_w psOutbuf Width of pixels in buffer, not less than (string length * character width)
 *@param[in]    unBuf_h psOutbuf  Heignt of pixels in buffer, pixel high, not less than the character height

 *@return
 *@li	\ref NDK_OK "NDK_OK"    Success
 *@li	\ref NDK_ERR_PARA     "NDK_ERR_PARA"        Invalid parameter
 *@li	\ref NDK_ERR_NOT_SUPPORT  "NDK_ERR_NOT_SUPPORT" Unknown font size
*/
int NDK_PrnGetDotData(EM_PRN_GETFONT emGetFont, const char *pszBuf, char *psOutbuf, uint unBuf_w, uint unBuf_h);

/**
 *@brief	  Load TTF font
 *@param[in]	  filepathname TTF font file path name
 *@param[in]	  face_index Face index in font file, set 0 if only one
 *@return
 *@li	\ref NDK_OK "NDK_OK"    Success
 *@li	\ref NDK_ERR_PARA     "NDK_ERR_PARA"        Invalid parameter
 *@li	\ref NDK_ERR                "NDK_ERR"       Invalid file format
*/
int NDK_PrnInitTrueTypeFont(const char* filepathname, int face_index);

/**
 *@brief    Set width and height of TTF font
 *@param[in]    width    Font width
 *@param[in]    height   Font height
 *@return
 *@li	\ref NDK_OK "NDK_OK"    Success
 *@li	\ref NDK_ERR	"NDK_ERR"  Fail
*/
int NDK_PrnSetTrueTypeFontSizeInPixel(int width, int height);

/**
 *@brief    Get width and height of TTF font
 *@param[in]    str    Print string
 *@param[in]    stringLength    Print string length
 *@param[out]    width    Font width
 *@param[out]    height   Font height
 *@return
 *@li	\ref NDK_OK "NDK_OK"    Success
 *@li	\ref NDK_ERR	"NDK_ERR"  Fail
*/
int NDK_PrnGetTrueTypeFontSize(const char *str, int stringLength, int *width, int *height);

/**
 *@brief    Set encoding format
 *@param[in]    input \ref EM_PRNTTFINPUT "EM_PRNTTFINPUT"
 *@return
 *@li	\ref NDK_OK "NDK_OK"    Success
*/
int NDK_PrnSetTrueTypeFontEncodingFormat(EM_PRNTTFINPUT input);

/**
 *@brief	 Print string with TTF font
 *@param[in]	unX	X-coordinate of string
 *@param[in]	str	String to be printed in ASCII or UNICODE format
 *@param[in]	stringLength String length to be printed
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR                "NDK_ERR"       Invalid file format

*/
int NDK_PrnTrueTypeFontText(uint unX, const char *str, int stringLength);

/**
 *@brief    Destroy TTF font
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
*/
int NDK_PrnDestroyTrueTypeFont(void);

/** @}*/ // End of print

/** @addtogroup FileSystem
* @{
*/

/**
 *@brief 		Open file.
 *@param[in]   pszName File name
 *@param[in]   pszMode "r" (Read only) or "w"(Write only).
 *@return
 *@li	 File handle
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Fail
*/
int NDK_FsOpen(const char *pszName, const char *pszMode);

/**
 *@brief 		Close file.
 *@param[in]    nHandle File handle
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_FsClose(int nHandle);

/**
 *@brief 		Read file.
 *@param[in]    nHandle File handle
 *@param[in]    unLength	Length of bytes to be read
 *@param[out]   psBuffer	Buffer to save data
 *@return
 *@li	Actual length read
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		 Invalid parameter
 *@li	\ref NDK_ERR_READ "NDK_ERR_READ" 	 Fail
*/
int NDK_FsRead(int nHandle, char *psBuffer, uint unLength );

/**
 *@brief 		Write file.
 *@param[in]    nHandle File handle
 *@param[in]    psBuffer	Data buffer with data to be written
 *@param[in]    unLength	Length of bytes to be written
 *@return
 *@li	Actual length written
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	  Invalid parameter
 *@li	\ref NDK_ERR_WRITE "NDK_ERR_WRITE" 		Failed
*/
int NDK_FsWrite(int nHandle, const char *psBuffer, uint unLength );

/**
 *@brief 		Set read/write position
 *@param[in]    nHandle File handle
 *@param[in]    ulDistance New position.
 *@param[in]    unPosition SEEK_SET: From start of file.\n
			SEEK_CUR: From current position.\n
			SEEK_END: From end of file.\n
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_FsSeek(int nHandle, ulong ulDistance, uint unPosition );

/**
 *@brief 		Delete file
 *@param[in]   pszName File name
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	   Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 			  Fail
*/
int NDK_FsDel(const char *pszName);

/**
 *@brief 		Get file length
 *@param[in]    pszName File name
 *@param[out]   punSize File length
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	  Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 				  Fail
*/
int NDK_FsFileSize(const char *pszName, uint *punSize);

/**
 *@brief 		Rename file
 *@param[in]    pszSrcName Old file name
 *@param[in]    pszDstName New file name
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		  Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 	   Fail
*/
int NDK_FsRename(const char *pszSrcName, const char *pszDstName );

/**
 *@brief 		Check if file exists
 *@param[in]    pszName File name
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success (File exists)
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	  Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 		  Fail
*/

int NDK_FsExist(const char *pszName);

/**
 *@brief 		Truncate file
 *@details   It will change file length to unLen. If previous file length is larger than unLen, it will be truncated. If previous file length is smaller than unLen, it will be padded with 0xff.
 *@param[in]    pszPath File path
 *@param[in]    unLen New file length
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		 Invalid parameter
 *@li	\ref NDK_ERR_PATH "NDK_ERR_PATH" 	Invalid file path
 *@li	\ref NDK_ERR "NDK_ERR" 		 Fail
 *@li	\ref NDK_ERR_WRITE "NDK_ERR_WRITE" 		  Failed to write file
*/
int NDK_FsTruncate(const char *pszPath, uint unLen );

/**
 *@brief 	  	Get read/write position
 *@param[in]    nHandle File handle
 *@param[out]   pulRet Current read/write postion
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 		Fail
*/
int NDK_FsTell(int nHandle, ulong *pulRet);

/**
 *@brief 	  	Get used space
 *@param[in]    	unWhich 0: Used; 1: Free
 *@param[out]     pulSpace Amount of space used or free
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 			 Fail
*/
int NDK_FsGetDiskSpace(uint unWhich, ulong *pulSpace);

/**
 *@brief 		Create directory
 *@param[in]    pszName Directory name
 *@return
 *@li \ref NDK_OK "NDK_OK"  			Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 		          Fail
*/

int NDK_FsCreateDirectory(const char *pszName);

/**
 *@brief 		Delete directory.
 *@param[in]    pszName Directory name
 *@return
 *@li \ref NDK_OK "NDK_OK"  			Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		   Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 	 Fail
*/
int NDK_FsRemoveDirectory(const char *pszName);

/**
 *@brief 		Format file system (Not valid for Phoenix OS)
 *@return
 *@li \ref NDK_OK "NDK_OK"  			Success
 *@li	\ref NDK_ERR_NOT_SUPPORT "NDK_ERR_NOT_SUPPORT" 		Not supported
*/
int NDK_FsFormat(void);

/**
 *@brief 		List file in directory
 *@details	The size of psBuf must be large enough to avoid overflow, 20 bytes to store one file name. The first 19 bytes is file name, the last byte indicates directory or file (0: Normal file; 1: Directory)
 *@param[in]       psPath Directory to read
 *@param[out]      psBuf Buffer to store file name
 *@param[out]      punNum Total number of files under this directory
 *@return
 *@li \ref NDK_OK "NDK_OK"  			Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		 Invalid parameter
 *@li	\ref NDK_ERR_PATH "NDK_ERR_PATH" 		  Invalid file path
*/
int NDK_FsDir(const char *psPath, char *psBuf, uint *punNum);

/** @}*/ // End of file system

/** @addtogroup ApplicationManagement
* @{
*/

#define NDK_APP_NAME_MAX_LEN   32 /**<Maximum length of application name*/
#define NDK_APP_VER_MAX_LEN    16 /**<Maximum length of version*/
#define NDK_APP_BUILD_TIME_LEN 32 /**<Maximum length of creation time*/
#define NDK_APP_DATA_MAX_LEN   4096  /**<Maximum length of result data from child process*/
/**
 *@brief Application infomation
*/
typedef struct APPINFO {
        uchar 	szAppName[NDK_APP_NAME_MAX_LEN + 1];     /**<Name*/
        uchar	szVerBuf[NDK_APP_VER_MAX_LEN + 1];       /**<Version*/
        int	nSeriNo;                                 /**<Serial No.*/
        uchar	szBuildTime[NDK_APP_BUILD_TIME_LEN + 1]; /**<Build time*/
        int 	nIsMaster;                               /**<Master or not*/
        uint	sunReverse[3];
} ST_APPINFO;

/*
*@brief Directory types
*/
typedef enum {
        APPPATH_LIARARY,       /**<Private library*/
        APPPATH_SHARA_LIBRARY, /**<Shared library*/
        APPPATH_TMP            /**<Temparory directory*/
} EM_APPPATH;

typedef int (*CallbackMock)(int, void *, int, void *, int *);

typedef struct AppExitStatus
{
	int 	    code;   /**<Exit code, 0: Success; Non-zero: Fail*/
	const void* data;   /**<Result data*/
	int 	    cbData; /**<Result data length*/
} AppExitStatusT;

/**
 *@brief	Set event callback function
 *@param[in] NDK_EventMain	Callback function
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		 Invalid parameter
*/
int NDK_AppSetEventCallBack(CallbackMock NDK_EventMain);

/**
 *@brief	Run application
 *@param[in] pszAppName	 Application name
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	   Invalid parameter
 *@li	\ref NDK_ERR_APP_MAX_CHILD "NDK_ERR_APP_MAX_CHILD" 	 Exceed the maximum number of running applications
 *@li	\ref NDK_ERR "NDK_ERR" 	   Fail
 *@li	\ref NDK_ERR_APP_NOT_EXIST "NDK_ERR_APP_NOT_EXIST" 	  Application not exist
 *@li	\ref NDK_ERR_READ "NDK_ERR_READ" 	Failed to read file
 *@li	\ref NDK_ERR_WRITE "NDK_ERR_WRITE" 	 Failed to write file
 *@li	\ref NDK_ERR_APP_CREAT_CHILD "NDK_ERR_APP_CREAT_CHILD" 	 	Failed to wait for child to exit
*/
int NDK_AppRun(const uchar *pszAppName);

/**
 *@brief	Handle event
 *@param[in] pszFileName		Application name
 *@param[in] nModuleId		Event ID
 *@param[in] pvInEventMsg	Input event message
 *@param[in] nInlen			Length of input event
 *@param[in] nMaxOutLen		Length of output event allowed
 *@param[out]	pvOutEventMsg	Output event
 *@param[out]	pnOutLen		Length of output event
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	Invalid parameter
 *@li	\ref NDK_ERR_APP_MAX_CHILD "NDK_ERR_APP_MAX_CHILD" 	 Exceed the maximum number of running applications
 *@li	\ref NDK_ERR "NDK_ERR" 	  Fail
 *@li	\ref NDK_ERR_APP_NOT_EXIST "NDK_ERR_APP_NOT_EXIST" 	 Application not exist
 *@li	\ref NDK_ERR_READ "NDK_ERR_READ" 	Failed to read file
 *@li	\ref NDK_ERR_WRITE "NDK_ERR_WRITE" 	 Failed to write file
 *@li	\ref NDK_ERR_APP_CREAT_CHILD "NDK_ERR_APP_CREAT_CHILD" 	  Failed to wait for child to exit
*/
int NDK_AppDoEvent(const uchar *pszFileName, int nModuleId, const void *pvInEventMsg, int nInlen, void *pvOutEventMsg, int nMaxOutLen, int *pnOutLen);

/**
 *@brief	Check if it needs reboot after installing application
 *@param[in] pnRebootFlag		1: Reboot needed; 0: Reboot not needed
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	  Invalid parameter
*/
int NDK_AppGetReboot(int *pnRebootFlag);

/**
 *@brief	Load application
 *@param[in] pszFileName		Application name
 *@param[in] nRebootFlag		Need reboot or not (Not valid for Phoenix OS)
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_APP_FILE_STAT "NDK_ERR_APP_FILE_STAT" 	   Failed to get file information
 *@li	\ref NDK_ERR_APP_FILE_OPEN "NDK_ERR_APP_FILE_OPEN" 		Failed to open file
 *@li	\ref NDK_ERR_APP_FILE_READ "NDK_ERR_APP_FILE_READ" 		 Failed to read file
 *@li	\ref NDK_ERR_APP_FILE_WRITE "NDK_ERR_APP_FILE_WRITE" 	Failed to write file
 *@li	\ref NDK_ERR_APP_MALLOC "NDK_ERR_APP_MALLOC" 		  Out of memory
 *@li	\ref NDK_ERR_APP_NLD_HEAD_LEN "NDK_ERR_APP_NLD_HEAD_LEN"  Failed to get the header information length of NLD file
 *@li	\ref NDK_ERR_APP_SIGN_CHECK "NDK_ERR_APP_SIGN_CHECK" 	 Failed to verify the signature data
 *@li	\ref NDK_ERR_APP_SIGN_DECRYPT "NDK_ERR_APP_SIGN_DECRYPT" 	 Failed to decrypt the signature data
 *@li	\ref NDK_ERR_APP_CERT_HAS_EXPIRED "NDK_ERR_APP_CERT_HAS_EXPIRED" 	Certification expired
 *@li	\ref NDK_ERR_APP_CERT_NOT_YET_VALID "NDK_ERR_APP_CERT_NOT_YET_VALID"  Certification not valid
*/
int NDK_AppLoad(const uchar *pszFileName, int nRebootFlag);

/**
 *@brief	 Get application info
 *@param[in] pszAppName	Application name.
 *@param[in] nPos		Offset in application list
 *@param[in] nSizeofInfo	Maximum length of application info
 *@param[out]	pstAppInfo	Buffer to save application info
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 	   Fail
 *@li	\ref NDK_ERR_APP_NOT_EXIST "NDK_ERR_APP_NOT_EXIST" 		Application not exist
*/
int NDK_AppGetInfo(const uchar *pszAppName, int nPos, ST_APPINFO *pstAppInfo, int nSizeofInfo);

/**
 *@brief	 Delete application
 *@param[in] pszAppName	Application name
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 	 Fail
 *@li	\ref NDK_ERR_APP_NOT_EXIST "NDK_ERR_APP_NOT_EXIST" 	 	Application not exist
*/
int NDK_AppDel(const char *pszAppName);

/**
 *@brief	Run application in background
 *@param[in] pszAppName	Application name.
 *@param[in] psArgv Parameters for application
 *@param[in] cBlock:  1: Block; 0: Non-block

 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_APP_NOT_EXIST "NDK_ERR_APP_NOT_EXIST" 		Application not exist
 *@li	\ref NDK_ERR_READ "NDK_ERR_READ" 		Failed to read file
 *@li	\ref NDK_ERR_WRITE "NDK_ERR_WRITE" 		Failed to write file
*/
int NDK_AppEXECV(const uchar *pszAppName, char * const psArgv[], char cBlock);

/**
 *@brief    Run process in background and get result data
 *@details  Work with \ref NDK_AppChildProcessExit "NDK_AppChildProcessExit", NDK_AppStartChildProcess is called by parent process, NDK_AppChildProcessExit is called by child process (Not thread-safe and recommend to use in block mode)
 *@param[in]    path Application name or executable binary
 *@param[in]    psArgv Command parameters
 *@param[in]    nLen  Length of data pointed by psArgv
 *@param[out]   exitStatusP Exit code and result data from child process

 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li \ref NDK_ERR_SHM "NDK_ERR_SHM"       Failed to allocate shared memory
*/
int NDK_AppStartChildProcess(const uchar *path, void *psArgv, int nLen, AppExitStatusT ** exitStatusP);

/**
 *@brief      Exit from child process with status and result data
 *@details    Work with \ref NDK_AppStartChildProcess "NDK_AppStartChildProcess"
 *@param[out]	exitCode  0 - Success; Non-zero: Fail
 *@param[out]   exitData Result data
 *@param[out]	cbData  Result data length
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li \ref NDK_ERR_SHM "NDK_ERR_SHM"       Failed to allocate shared memory
*/
int NDK_AppChildProcessExit(int exitCode, const void* exitData, int cbData);

/**
 *@brief     Get data from parent
 *@details   Work with \ref NDK_AppStartChildProcess "NDK_AppStartChildProcess" (Not thread-safe)
 *@param[out]	pData Data returned from parent
 *@param[out]	pnLen Date length
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li \ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li \ref NDK_ERR_SHM "NDK_ERR_SHM"       Failed to allocate shared memory
 *@li \ref NDK_ERR "NDK_ERR"       Failed to free shared memory
*/
int NDK_AppGetDatafromParentProcess(void *pData, int * pnLen);

/**
 *@brief    Get application directory.
 *@param[in]    emPathId  Directory ID
 *@param[out]   pszPath   Buffer to save application directory (End with '\')
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li \ref NDK_ERR_PARA "NDK_ERR_PARA"        Invalid parameter
*/
int NDK_AppGetPathById(EM_APPPATH emPathId, char *pszPath);

/**
 *@brief    Get installation directory.
 *@param[in]    pszAppName  Application name
 *@param[out]   pszAppPath   Buffer to save application installation directory (End with '\')
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"          Invalid parameter
 *@li   \ref NDK_ERR_APP_FILE_NOT_EXIST "NDK_ERR_APP_FILE_NOT_EXIST"       Application not exist
*/
int NDK_AppGetPathByAppName(const char *pszAppName, char *pszAppPath);

/** @}*/ //  End of application management

/** @addtogroup Crypto
* @{
*/

#define MAX_RSA_MODULUS_LEN		512 /**<Maximum modulus length*/
#define MAX_RSA_PRIME_LEN		256 /**<Maximum prime length*/

/**
 *@brief Symmetric key algorithms
*/
typedef enum {
        ALG_TDS_MODE_ENC = 0, /**<DES encryption*/
        ALG_TDS_MODE_DEC = 1, /**<DES decryption*/
} EM_ALG_TDS_MODE;

/**
 *@brief Length of key for RSA algorithm
*/
typedef enum {
        RSA_KEY_LEN_512  = 512,
        RSA_KEY_LEN_1024 = 1024,
        RSA_KEY_LEN_2048 = 2048,
} EM_RSA_KEY_LEN;

/**
 *@brief Exponents for RSA algorithm
*/
typedef enum {
        RSA_EXP_3     = 0x03,
        RSA_EXP_10001 = 0x10001,
} EM_RSA_EXP;

/**
 *@brief RSA public key
*/
typedef struct {
        ushort bits;                                   /**<Modulus in bits*/
        uchar modulus[MAX_RSA_MODULUS_LEN + 1];        /**<Modulus length*/
        uchar publicExponent[MAX_RSA_MODULUS_LEN + 1]; /**<Public key exponent*/
        uchar exponent[MAX_RSA_MODULUS_LEN + 1];       /**<Private key exponent*/
        uchar prime[2][MAX_RSA_PRIME_LEN + 1];         /**<Prime number*/
        uchar primeExponent[2][MAX_RSA_PRIME_LEN + 1]; /**<Division value of prime number and exponent*/
        uchar coefficient[MAX_RSA_PRIME_LEN + 1];      /**<Division value of prime number and prime number*/
} ST_RSA_PRIVATE_KEY;

/**
 *@brief RSA private key
*/
typedef struct {
        ushort bits;                             /**<Modulus in bits*/
        uchar modulus[MAX_RSA_MODULUS_LEN + 1];  /**<Modulus length*/
        uchar exponent[MAX_RSA_MODULUS_LEN + 1]; /**<Exponent*/
} ST_RSA_PUBLIC_KEY;

/**
 *@brief 	Calculate 3DES
 *@param[in] psDataIn	Buffer for data to encrypt
 *@param[in] psKey		Key buffer
 *@param[in] nKeyLen     Key length (8, 16 or 24 only)
 *@param[in] nMode		Encryption mode
 *@param[out]	psDataOut	Output data
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li \ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_AlgTDes(uchar *psDataIn, uchar *psDataOut, uchar *psKey, int nKeyLen, int nMode);

/**
 *@brief	Calculate SHA1
 *@param[in] psDataIn	Input data
 *@param[in] nInlen		Input data length
 *@param[out]	psDataOut	Output data (20 bytes)
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"  Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 			Fail
*/
int NDK_AlgSHA1(uchar *psDataIn, int nInlen, uchar *psDataOut);

/**
 *@brief		Calculate SHA256
 *@param[in] psDataIn	Input number
 *@param[in] nInlen		Input data length
 *@param[out]	psDataOut	Output data
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"  Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 			Fail
*/
int NDK_AlgSHA256(uchar *psDataIn, int nInlen, uchar *psDataOut);

/**
 *@brief		Calculate SHA512
 *@param[in] psDataIn	Input number
 *@param[in] nInlen		Input data length
 *@param[out]	psDataOut	Output data
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"   Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 	  Fail
*/
int NDK_AlgSHA512(uchar *psDataIn, int nInlen, uchar *psDataOut);

/**
 *@brief	Generate RSA key pairs
 *@param[in] nProtoKeyBit		Key length: 512, 1024 and 2048 bits. (\ref EM_RSA_KEY_LEN "EM_RSA_KEY_LEN")
 *@param[in] nPubEType			Exponent types (\ref EM_RSA_EXP "EM_RSA_EXP")
 *@param[out]	pstPublicKeyOut		Public key
 *@param[out]	pstPrivateKeyOut	Private key
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 			Fail
*/
int NDK_AlgRSAKeyPairGen( int nProtoKeyBit, int nPubEType, ST_RSA_PUBLIC_KEY *pstPublicKeyOut, ST_RSA_PRIVATE_KEY *pstPrivateKeyOut);

/**
 *@brief	Encrypt/Decrypt with RSA key pair
 *@details	If  psModule and psExp is private key, then do encryption, if it's public key, do decryption. 1st byte of psDataIn must be less than 1st byte of psModule. This function can do RSA computation equals or less than 2048 bits.
 *@param[in] psModule		Module buffer
 *@param[in] nModuleLen		Module length
 *@param[in] psExp			Bbuffer that stores exponents for RSA algorithm(Big-endian)
 *@param[in] psDataIn		Input data
 *@param[out]	psDataOut		Output data
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 			Fail
*/
int NDK_AlgRSARecover(uchar *psModule, int nModuleLen, uchar *psExp, uchar *psDataIn, uchar *psDataOut);

/**
 *@brief	Check RSA key pair
 *@param[in] pstPublicKey		Public key
 *@param[in] pstPrivateKey		Private key
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 		Fail
*/
int NDK_AlgRSAKeyPairVerify(ST_RSA_PUBLIC_KEY *pstPublicKey, ST_RSA_PRIVATE_KEY *pstPrivateKey);

/** @}*/ // End of algorithm

/** @addtogroup Security
* @{
*/

/**
 *@brief Key types
*/
typedef enum {
        SEC_KEY_TYPE_TLK = 0, /**<Terminal load key*/
        SEC_KEY_TYPE_TMK,     /**<Master key*/
        SEC_KEY_TYPE_TPK,     /**<PIN key*/
        SEC_KEY_TYPE_TAK,     /**<MAC key*/
        SEC_KEY_TYPE_TDK,     /**<Data encryption/decryption key*/
	SEC_KEY_TYPE_RESP_TAK, /*<DUKPT Response MAC key*/
	SEC_KEY_TYPE_RESP_TDK, /*<DUKPT Response DATA key*/
} EM_SEC_KEY_TYPE;

/**
 *@brief Key check modes
*/
typedef enum {
        SEC_KCV_NONE = 0, /**<No check*/
        SEC_KCV_ZERO,     /**<Run DES/TDES encryption algorithm with 0x00 of 8 bytes, and first 4 bytes of ciphertext is obtained, which is KCV*/
        SEC_KCV_VAL,      /**<First, run odd parity with plaintext of key, then run DES/TDES algorithem with "\x12\x34x56\x78\x90\x12\x34\x56", first 4 bytes of ciphertext is obtained, which is KCV*/
        SEC_KCV_DATA,     /**<Send in a string of data KcvData, run specific mode of MAC algorithm to [aucDstKeyValue(ciphertext) + KcvData] with source key pair, MAC of 8 bytes is obtained, which is KCV*/
} EM_SEC_KCV;

/**
 *@brief MAC algorithms
*/
typedef enum {
        SEC_MAC_X99 = 0,    /**<X99 algorithm:o data is divided into blocks of 8 bytes, supplement with 0 if there is vacancy, exclusive or (XOR) of each encrypted block with next block, and then encrypt the result per key length*/
        SEC_MAC_X919,       /**<X99 algorithm: data is divided into blocks of 8 bytes, suppliment with 0 if vacancy, exclusive or (XOR) of each encrypted block with next block, and then encrypt the result per key DES,
                            last frame, if key is of length 16 bytes, then per 3DES, 8 bytes per DES*/
        SEC_MAC_ECB,        /**<After running exclusive or (XOR) operation on all data, proceed DES conversion with data, refer to UnionPay regulations on ECB algorithm*/
        SEC_MAC_9606,       /**<After exclusive or (XOR) of all data, run des algorithnon with data*/

	SEC_MAC_RESP_X99 = 0x20,	/*New DUKPT response mac*/
	SEC_MAC_RESP_X919,
	SEC_MAC_RESP_ECB,
	SEC_MAC_RESP_9606,
} EM_SEC_MAC;

/**
 *@brief PIN block modes
*/
typedef enum {
        SEC_PIN_ISO9564_0 = 3, /**<Lack of encryption, using master account password number fill 'F'*/
        SEC_PIN_ISO9564_1 = 4, /**<Do not use the master account encryption, code digits for the random number*/
        SEC_PIN_ISO9564_2 = 5, /**<Do not encrypt with master account, fill in with 'F' to complete the length if password is too short*/
        SEC_PIN_ISO9564_3 = 6  /**<Encrypt with master account, fill in with random numbers to complete the length if password is too short*/
} EM_SEC_PIN;

/**
 *@brief DES algorithm types. Use OR operation with different bits, e.g.: SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8|SEC_DES_ECBMODE means to encrypt in ECB mode with 8 bytes key length .
*/
typedef enum {
        SEC_DES_ENCRYPT        = 0,        /**<DES encription*/
        SEC_DES_DECRYPT        = 1,        /**<DES decryption*/
        SEC_DES_KEYLEN_DEFAULT = (0 << 1), /**<Encrypt with key length installed*/
        SEC_DES_KEYLEN_8       = (1 << 1), /**<Encrypt with 8 bytes key*/
        SEC_DES_KEYLEN_16      = (2 << 1), /**<Encrypt with 16 bytes key*/
        SEC_DES_KEYLEN_24      = (3 << 1), /**<Encrypt with 24 bytes key*/
        SEC_DES_MODE_ECB       = (0 << 3), /**<Encrypt with ECB mode*/
        SEC_DES_MODE_CBC       = (1 << 3), /**<Encrypt with CBC mode*/
} EM_SEC_DES;

/**
 *@brief Key values returned by VPP
*/
typedef enum {
        SEC_VPP_KEY_PIN,       /**<PIN key pressed*/
        SEC_VPP_KEY_BACKSPACE, /**<Backspace key pressed*/
        SEC_VPP_KEY_CLEAR,     /**<Clear key pressed*/
        SEC_VPP_KEY_ENTER,     /**<Enter key pressed*/
        SEC_VPP_KEY_ESC,       /**<PIN input cancelled*/
        SEC_VPP_KEY_NULL       /**<No key pressed*/
} EM_SEC_VPP_KEY;

typedef enum {
        SEC_OFFLINE_PIN_MODE_EMV = 0, /**<Offline PIN mode*/
} ST_SEC_OFFLINE_PIN_MODE;

/**
 *@brief Tamper states
*/
typedef enum {
        SEC_TAMPER_STATUS_NONE       = 0,        /**<No tamper*/
        SEC_TAMPER_STATUS_HW         = (1 << 0), /**<Hardware tamper*/
        SEC_TAMPER_STATUS_SEC_CONFIG = (1 << 1), /**<Security register value changed*/
        SEC_TAMPER_STATUS_CHECKFILE  = (1 << 2), /**<File checksum validation error*/
        SEC_TAMPER_DEVICE_DISABLED   = (1 << 8), /**<Deviced not enabled*/
} EM_SEC_TAMPER_STATUS;

#define SEC_KEYBLOCK_FMT_TR31		(0x54523331)	/**<Extension of TR-31 key block format, 0x54523331 means "TR31"*/

/**
 *@brief Extended key installation package information, used to implement the TR - 31 and extended key installation package format. When the user wants to use TR - 31 encapsulated package to install key, will need to key the data encapsulated into ST_EXTEND_KEYBLOCK structure and stored in the structure of ST_SEC_KEY_INFO sDstKeyValue [24] members, passed to key installation interface, the system will try to use the format parses the installation key.
*/
typedef struct {
        unsigned int format; /**<Extend key installation package format, currently only support SEC_KEYBLOCK_FMT_TR31 TR-31 format*/
        unsigned int len;    /**<Key installation package data (pblock) length*/
        char *pblock;        /**<The key data pointer*/
} ST_EXTEND_KEYBLOCK;

/**
 *@brief Key information
*/
typedef struct {
        uchar 	ucScrKeyType;     /**<Type of source key that diffused this key (\ref EM_SEC_KEY_TYPE "EM_SEC_KEY_TYPE"), it shall not be lower than key level ucDstKeyType is at*/
        uchar 	ucDstKeyType;     /**<Type of destination key (\ref EM_SEC_KEY_TYPE "EM_SEC_KEY_TYPE")*/
        uchar 	ucScrKeyIdx;      /**<Index of source key that diffused this key, typically index starts from 1, if this variable equals 0, then this key is written in plaintext*/
        uchar 	ucDstKeyIdx;      /**<Index of destination key, range [1 - 255], DO NOT USE 0*/
        int 	nDstKeyLen;       /**<Length of destination key:8, 16, 24*/
        uchar 	sDstKeyValue[24]; /**<Key value to write*/
} ST_SEC_KEY_INFO;

/**
 *@brief Key check infromation
*/
typedef struct {
        int 	nCheckMode;     /**<Check mode (\ref ST_SEC_KCV_INFO "ST_SEC_KCV_INFO")*/
        int 	nLen;           /**<Data length to be checked*/
        uchar 	sCheckBuf[128]; /**<Data buffer*/
} ST_SEC_KCV_INFO;

/**
 *@brief Offline encrypted PIN key
*/
typedef struct {
        uint	unModlen;                  /**<Length of modulus for public encrytion key*/
        uchar	sMod[MAX_RSA_MODULUS_LEN]; /**<Modulus for public key for encryption, big-endian, fill in with 0 if vacancy*/
        uchar	sExp[4];                   /**<Modulus for public key for encryption, big-endian, fill in with 0 if vacancy*/
        uchar	ucIccRandomLen;            /**<Length of random number get from card*/
        uchar	sIccRandom[8];             /**<Random number get from card*/
} ST_SEC_RSA_PINKEY;

/**
 *@brief RSA key information
*/
typedef struct {
        ushort usBits;                            /**<Number of bits*/
        uchar sModulus[MAX_RSA_MODULUS_LEN + 1];  /**<Modulus*/
        uchar sExponent[MAX_RSA_MODULUS_LEN + 1]; /**<Exponent*/
        uchar reverse[4];
} ST_SEC_RSA_KEY;

/**
 *@brief	Read security module version
 *@param[out]	pszVerInfoOut	Buffer to save version (Less than 16 bytes)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	   Invalid parameter
*/
int NDK_SecGetVer(uchar * pszVerInfoOut);

/**
 *@brief	Get random number
 *@param[in] nRandLen		Length needed
 *@param[out]	pvRandom		Buffer to save random number
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	  Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 			Fail
*/
int NDK_SecGetRandom(int nRandLen, void *pvRandom);

/**
 *@brief	Get KCV value of key
 *@details	Used to check key for both part of session
 *@param[in] ucKeyType		Key type
 *@param[in] ucKeyIdx		Key index
 *@param[in] pstKcvInfoOut	KCV encryption mode
 *@param[out]	pstKcvInfoOut	KCV value
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		  Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 			Fail
*/
int NDK_SecGetKcv(uchar ucKeyType, uchar ucKeyIdx, ST_SEC_KCV_INFO *pstKcvInfoOut);

/**
 *@brief	 Delate all keys
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_SecKeyErase(void);

/**
 *@brief	                Delate key
 *@param[in] ucKeyIdx		Key index
 *@param[in] ucKeyType	  Key Type (\ref EM_SEC_KEY_TYPE "EM_SEC_KEY_TYPE")
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_SecKeyDelete(uchar ucKeyIdx, uchar ucKeyType);

/**
*@brief	Install key
 *@details Include writing and diffusing TLK, TMK and TWK, also choose to check correctness of key with KCV. Key length must be more than 8 bytes. \n
 	PED adopts 3-layer key system, and layers from top down are:\n
	TLK-Terminal Key Loading Key\n
    	private key held by acquiring bank or POS operator which is directly written by its holder under secured conditions.\n
    	each PED terminal has only one such key, so the index No. of key is from 1 to 1.\n

	TMK-Terminal Master Key=Acquirer Master Key\n
		is master key for terminal or called master key for acquiring bank   keys of this type can amount to 100 in number, so the index no. is from 1 to 100.\n
		TMK can be directly written under secured conditions, write TMK directly, and make it consistent with MK/SK key system by diffusing TWK.\n
	TWK-Transaction working key = Transaction Pin Key + Transaction MAC Key + Terminal DES Enc Key + Terminal DES DEC/ENC Key\n
		Terminal working key for PIN ciphertext, MAC and other operations. keys of this type can amount to 100 in number, so the index no. is from 1 to 100.\n
		TPK: is to calculate PIN Block after application inputs PIN.\n
		TAK: is to calculate MAC for message communication in the application.\n
		TEK: is to provide DES/TDES encrypted transmission or storage for sensitive data in the application.\n
		TEK: is to provide DES/TDES encryption or decryption for sensitive data in the application.\n
	TWK can be written under secured conditions, TWK directly written is consistent with Fixed Key system. Each key has its index No., length, usage and tag.\n
	wherein key tag is set via API before key is written, in order to authorize this key for use and ensure it won't be misused.\n

	DUKPT key mechanism:\n
	DUKPT[Derived Unique Key Per Transaction]key management system is designed for providing one unique key for one transaction, and the working key[PIN,MAC]for each transaction differs.\n
	It has introduced KSN [Key Serial Number] concept, KSN is the essential factor to achieve one transaction one key. Each KSN corresponds to a key, different keys can be generated per usage.\n
 	10 groups of such keys are allowed, user has to select index No. of group when writing TIK and select proper group index when DUKPT key is used
 *@param[in] pstKeyInfoIn		Key info
 *@param[in] pstKcvInfoIn		Key check info
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	Invalid parameter
 *@li	\ref NDK_ERR_MACLLOC "NDK_ERR_MACLLOC" 	Out of memory
 *@li	\ref NDK_ERR "NDK_ERR" 		             Fail
*/
int NDK_SecLoadKey(ST_SEC_KEY_INFO * pstKeyInfoIn, ST_SEC_KCV_INFO * pstKcvInfoIn);

/**
 *@brief	Calculate MAC
 *@param[in] ucKeyIdx		Key index
 *@param[in] psDataIn		Input data
 *@param[in] nDataInLen		Input data length
 *@param[in] ucMod			MAC calculation mode (\ref EM_SEC_MAC "EM_SEC_MAC")
 *@param[out]	psMacOut		MAC value (8 bytes)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR "NDK_ERR" 		Fail
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_MACLLOC "NDK_ERR_MACLLOC" 		Out of memory
*/
int NDK_SecGetMac(uchar ucKeyIdx, uchar *psDataIn, int nDataInLen, uchar *psMacOut, uchar ucMod);

/**
 *@brief	Get PIN Block
 *@param[in] ucKeyIdx		Key index
 *@param[in] pszExpPinLenIn	 Password length (Can be separated as 0, 4, 6)
 *@param[in] pszDataIn		PIN BLOCK per ISO9564
 *@param[in] ucMode			Calculation mode (\ref EM_SEC_PIN "EM_SEC_PIN")
 *@param[in] nTimeOutMs		Timeout in milliseconds
 *@param[out]	psPinBlockOut	    Outpur PIN Block
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR "NDK_ERR" 		 Fail
 *@li	\ref NDK_ERR_MACLLOC "NDK_ERR_MACLLOC" 		Out of memory
 *@li	\ref NDK_ERR_SECP_PARAM "NDK_ERR_SECP_PARAM" 	 Invalid parameter
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 			  Invalid parameter
*/
int NDK_SecGetPin(uchar ucKeyIdx, uchar *pszExpPinLenIn, const uchar * pszDataIn, uchar *psPinBlockOut, uchar ucMode, uint nTimeOutMs);

/**
 *@brief	 Calculate DES
 *@details 	 Do DES calculation with specified key index 1-100
 *@param[in] ucKeyType		DES key type
 *@param[in] ucKeyIdx		DES key index
 *@param[in] psDataIn		Input data
 *@param[in] nDataInLen		Input data length
 *@param[in] ucMode			Encryption mode (\ref EM_SEC_DES "EM_SEC_DES")
 *@param[out]	psDataOut		Output data
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR"		 Fail
 *@li	\ref NDK_ERR_SECP_PARAM "NDK_ERR_SECP_PARAM" 	Invalid parameter
 *@li	\ref NDK_ERR_MACLLOC "NDK_ERR_MACLLOC" 	 Out of memory
*/
int NDK_SecCalcDes(uchar ucKeyType, uchar ucKeyIdx, uchar * psDataIn, int nDataInLen, uchar *psDataOut, uchar ucMode);

/**
 *@brief Verify offline plaintext PIN
 *@details 	 Send plaintext PIN BlOCK directly to card.
 *@param[in] ucIccSlot		Smart card slot
 *@param[in] pszExpPinLenIn	Password length, can be separated as 0, 4, 6
 *@param[in] ucMode			Calculation mode (Support EMV only)
 *@param[in] unTimeoutMs		Timeout in milliseconds
 *@param[out]	psIccRespOut	Response code from smart card
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 			 Invalid parameter
 *@li	\ref NDK_ERR_SECP_PARAM "NDK_ERR_SECP_PARAM" 	 Invalid parameter
 *@li	\ref NDK_ERR_MACLLOC "NDK_ERR_MACLLOC" 	               Out of memory
 *@li	\ref NDK_ERR "NDK_ERR" 	                           Fail
*/
int NDK_SecVerifyPlainPin(uchar ucIccSlot, uchar *pszExpPinLenIn, uchar *psIccRespOut, uchar ucMode,  uint unTimeoutMs);

/**
 *@brief	   Verify offline ciphertext PIN
 *@details 	Encrypt plaintext PIN with RsaPinKey per EMV spec and then send ciphertext PIN directly to card.
 *@param[in] ucIccSlot		Smart card slot
 *@param[in] pszExpPinLenIn	Password length, can be separated as 0, 4, 6
 *@param[in] pstRsaPinKeyIn	RSA key data
 *@param[in] ucMode			Calculation mode (Support EMV only)
 *@param[in] unTimeoutMs		Timeout in milliseconds
 *@param[out]	psIccRespOut	Response code from smart card
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				    Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 			 Invalid parameter
 *@li	\ref NDK_ERR_SECP_PARAM "NDK_ERR_SECP_PARAM" 		 Invalid parameter
 *@li	\ref NDK_ERR_MACLLOC "NDK_ERR_MACLLOC" 	           Out of memory
 *@li	\ref NDK_ERR "NDK_ERR" 	                Fail
*/
int NDK_SecVerifyCipherPin(uchar ucIccSlot, uchar *pszExpPinLenIn, ST_SEC_RSA_KEY *pstRsaPinKeyIn, uchar *psIccRespOut, uchar ucMode, uint unTimeoutMs);

/**
 *@brief	Install DUKPT key
 *@param[in] ucGroupIdx		Key group index
 *@param[in] ucSrcKeyIdx		Old key index (Key index that is used to encrypt initial key value)
 *@param[in] ucKeyLen		Key length
 *@param[in] psKeyValueIn	Initial key value
 *@param[in] psKsnIn		    KSN value
 *@param[in] pstKcvInfoIn	KCV info
 *@return
 *@li	\ref NDK_OK "NDK_OK"  		               Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		   Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 		               Fail
 *@li	\ref NDK_ERR_MACLLOC "NDK_ERR_MACLLOC"           Out of memory
*/
int NDK_SecLoadTIK(uchar ucGroupIdx, uchar ucSrcKeyIdx, uchar ucKeyLen, uchar * psKeyValueIn, uchar * psKsnIn, ST_SEC_KCV_INFO * pstKcvInfoIn);

/**
 *@brief	 Get KSN
 *@param[in] ucGroupIdx		Key group index
 *@param[out]	psKsnOut		Current KSN
 *@return
 *@li	\ref NDK_OK "NDK_OK"  		Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		 Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 			 Fail
*/
int NDK_SecGetDukptKsn(uchar ucGroupIdx, uchar * psKsnOut);

/**
 *@brief	 Increase KSN
 *@param[in] ucGroupIdx		Key group index
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_SecIncreaseDukptKsn(uchar ucGroupIdx);

/**
 *@brief	 Get PIN Block with DUKPT key
 *@param[in] ucGroupIdx		Key group index
 *@param[in] pszExpPinLenIn	Password length (Can be separated as 0, 4, 6)
 *@param[in] psDataIn		Input PIN block per ISO9564
 *@param[in] ucMode			Calculation mode
 *@param[in] unTimeoutMs		Timeout in milliseconds
 *@param[out]	psKsnOut		Current KSN
 *@param[out]	psPinBlockOut	Output PIN Block
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				   Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 			Invalid parameter
 *@li	\ref NDK_ERR_SECP_PARAM "NDK_ERR_SECP_PARAM" 		Invalid parameter
 *@li	\ref NDK_ERR_MACLLOC "NDK_ERR_MACLLOC" 	          Out of memory
 *@li	\ref NDK_ERR "NDK_ERR" 	                  Fail
*/
int NDK_SecGetPinDukpt(uchar ucGroupIdx, uchar *pszExpPinLenIn, uchar * psDataIn, uchar* psKsnOut, uchar *psPinBlockOut, uchar ucMode, uint unTimeoutMs);

/**
 *@brief	 Calculate MAC with DUKPT key
 *@param[in] ucGroupIdx		Key group index
 *@param[in] psDataIn		Input data
 *@param[in] nDataInLen		Input data length
 *@param[in] ucMode			MAC calculation mode
 *@param[out]	psMacOut		Output MAC value (8 bytes)
 *@param[out]	psKsnOut		Current KSN
 *@return
 *@li	\ref NDK_OK "NDK_OK"  			  Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_MACLLOC "NDK_ERR_MACLLOC" 	 Out of memory
 *@li	\ref NDK_ERR "NDK_ERR"  	  Fail
*/
int NDK_SecGetMacDukpt(uchar ucGroupIdx, uchar *psDataIn, int nDataInLen, uchar *psMacOut, uchar *psKsnOut, uchar ucMode);

/**
 *@brief	 Calculate DES with DUKPT key
 *@param[in] ucGroupIdx		Key group index
 *@param[in] ucKeyVarType	Key type
 *@param[in] psIV			Initial vector
 *@param[in] psDataIn		Input data
 *@param[in] usDataInLen		Input data length
 *@param[in] ucMode			Encryption mode
 *@param[out]	psDataOut		Output data
 *@param[out]	psKsnOut		Current KSN
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_SECP_PARAM "NDK_ERR_SECP_PARAM"   Invalid parameter
 *@li	\ref NDK_ERR_MACLLOC "NDK_ERR_MACLLOC" 		 Out of memory
 *@li	\ref NDK_ERR "NDK_ERR" 		  Fail
*/
int NDK_SecCalcDesDukpt(uchar ucGroupIdx, uchar ucKeyVarType, uchar *psIV, ushort usDataInLen, uchar *psDataIn, uchar *psDataOut, uchar *psKsnOut, uchar ucMode);

/**
 *@brief	Get PIN input status (For DUKPT)
 *@param[out]	psPinBlock		Pinblock data
 *@param[out]	psKsn			 Current KSN
 *@param[out] 	nStatus			 PIN input status
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	  Invalid parameter
 *@li	\ref NDK_ERR_SECVP_NOT_ACTIVE "NDK_ERR_SECVP_NOT_ACTIVE" 		VPP not active
*/
int NDK_SecGetPinResultDukpt(uchar *psPinBlock, uchar *psKsn, int *nStatus);

/**
 *@brief	Get PIN input status
 *@param[out]	psPinBlock		Pinblock data in SEC_VPP_KEY_PIN, SEC_VPP_KEY_BACKSPACE, SEC_VPP_KEY_CLEAR, the length of input PIN is saved in the first byte.
 *@param[out] 	nStatus			PIN input status
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	  Invalid parameter
 *@li	\ref NDK_ERR_SECP_PARAM "NDK_ERR_SECP_PARAM"   Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR"          	   Fail
 *@li	\ref NDK_ERR_SECVP_NOT_ACTIVE "NDK_ERR_SECVP_NOT_ACTIVE" 	VPP not active
*/
int NDK_SecGetPinResult(uchar *psPinBlock, int *nStatus);

/**
 *@brief	Set key owner
 *@details 	Passing NULL or an empty string "" to restore key owner to the application.
 *@param[in] pszName		Key owner name (Less than 256)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		 Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 			Fail
*/
int NDK_SecSetKeyOwner(char *pszName);

/**
 *@brief	Get tamper status
 *@param[out]	pnStatus			Tamper status (\ref EM_SEC_TAMPER_STATUS "EM_SEC_TAMPER_STATUS")
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		 Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 			Fail
*/
int NDK_SecGetTamperStatus(int *pnStatus);

/**
 *@brief	Get time left before next reboot required from PCI 4.0
 *@details 	This function gets the time to reset as specified in PCI 4.0 (and above). PCI 4.0 specifies that device must be rebooted every 24 hours maximum. If application does not implements this rebooting sequence, firmware does perform that feature on its own side, this function retrieves how much time until it.\n
It is useful for applications in order to reboot terminal by its own when it is "idle". This is, when no critical action is running: payment transaction, batch process, updating tasks, sending reports to acquire.
 *@param[in] timeToReboot Time left before next reboot
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		 Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 			Fail
*/
int NDK_SecGetTimeToReboot( unsigned int *timeToReboot );

/** @}*/ // End of the security

/** @addtogroup SerialPort
* @{
*/

/**
 *@brief  Serial ports
*/
typedef enum {
        PORT_NUM_COM1     = 0,  /**<Serial port 1*/
        PORT_NUM_COM2     = 1,  /**<Serial port 2*/
        PORT_NUM_WIRELESS = 2,  /**<Wireless port*/
        PORT_NUM_MUX1     = 3,  /**<Multiplexing port 1*/
        PORT_NUM_MUX2     = 4,  /**<Multiplexing port 2*/
        PORT_NUM_MUX3     = 5,  /**<Multiplexing port 3*/
        PORT_NUM_MODEM    = 6,  /**<Modem port*/
        PORT_NUM_WIFI     = 7,  /**<WiFi port*/
        PORT_NUM_USB      = 8,  /**<USB port*/
        PORT_NUM_SCAN     = 9,  /**<Barcode scanner port*/
        PORT_NUM_USB_HOST = 10, /**<USB serial port (Host mode)*/
        PORT_NUM_BT       = 11  /**<Bluebooth serial port (Host mode)*/
} EM_PORT_NUM;


/**
 *@brief   Open serial port
 *@details Set baud rate,data bit, parity bit and stop bit and etc. Recommend to call this function every time before using serial port. \n
			 Baud rate supported: 300,1200,2400,4800,9600,19200,38400,57600,115200\n
 			Data bit supported: 8,7,6,5\n
 			Parity Check mode supported: N(n):no parity; O(o):odd; E(e):even\n
			Stop bit supported: 1,2
 *@param[in] emPort	Serial port number
 *@param[in] pszAttr	Initialization string,e.g. "115200,8,N,1". When baud rate is missing, "8,N,1" is default.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  		 Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	 Failed to open device
 *@li	\ref NDK_ERR "NDK_ERR" 			  Fail
*/
int NDK_PortOpen(EM_PORT_NUM emPort, const char *pszAttr);

/**
 *@brief	Close serial port
 *@param[in] emPort	Serial port number
 *@return
 *@li	\ref NDK_OK "NDK_OK"  			 Success
 *@li	\ref NDK_ERR "NDK_ERR"  	 Fail
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
*/
int NDK_PortClose(EM_PORT_NUM emPort);

/**
 *@brief	 Receive data
 *@param[in] emPort	Serial port number
 *@param[in] unLen	Length of data to read
 *@param[in] nTimeoutMs	Timeout in milliseconds
 *@param[out]	pszOutBuf	Buffer to save data
 *@param[out]	pnReadLen	Actual length read
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"  Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	 Failed to open device
 *@li	\ref NDK_ERR_USB_LINE_UNCONNECT "NDK_ERR_USB_LINE_UNCONNECT" 		 USB cable not connected
 *@li	\ref NDK_ERR_READ "NDK_ERR_READ" 		 Failed to read
 *@li	\ref NDK_ERR_TIMEOUT "NDK_ERR_TIMEOUT" 		 Timeout error
 *@li	\ref NDK_ERR "NDK_ERR" 			Fail
*/
int NDK_PortRead(EM_PORT_NUM emPort, uint unLen, char *pszOutBuf, int nTimeoutMs, int *pnReadLen);

/**
 *@brief	 Send data
 *@param[in] emPort	Serial port number
 *@param[in] unLen	Length of data to write
 *@param[in] pszInbuf	Data buffer to write
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	 Failed to open device
 *@li	\ref NDK_ERR_USB_LINE_UNCONNECT "NDK_ERR_USB_LINE_UNCONNECT" 	 USB cable not connected
 *@li	\ref NDK_ERR_WRITE "NDK_ERR_WRITE" 			Failed to write
 *@li	\ref NDK_ERR "NDK_ERR" 				Fail
*/
int NDK_PortWrite(EM_PORT_NUM emPort, uint unLen, const char *pszInbuf);

/**
 *@brief	 Check if send buffer is empty.
 *@param[in] emPort	Serial port number
 *@return
 *@li	\ref NDK_OK "NDK_OK"  	No data in buffer
 *@li	\ref NDK_ERR "NDK_ERR"	Data in buffer
*/
int NDK_PortTxSendOver(EM_PORT_NUM emPort);

/**
 *@brief	 Clear receive buffer.
 *@param[in] emPort	Serial port number
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	 Failed to open device
 *@li	\ref NDK_ERR "NDK_ERR" 				Fail
*/
int NDK_PortClrBuf(EM_PORT_NUM emPort);

/**
 *@brief Check if there is any data in receive buffer
 *@param[in] emPort	Serial port number
 *@param[out]	pnReadLen	Length of data in receiving buffer
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	 Failed to open device
 *@li	\ref NDK_ERR "NDK_ERR" 				Fail
*/
int NDK_PortReadLen(EM_PORT_NUM emPort, int *pnReadLen);

/**
 *@brief    Rest serial port.
 *@param[in]   emPort  Serial port number
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR "NDK_ERR" 				Fail

*/
int NDK_PortReset(EM_PORT_NUM emPort);

/** @}*/ // End of serial port


/** @addtogroup Modem
* @{
*/

/**
*@brief  Modem connection states
*/
typedef enum MDM_STATUS {
        MDMSTATUS_NORETURN_AFTERPREDIAL    = 0,
        MDMSTATUS_OK_AFTERPREDIAL          = 1,
        MDMSTATUS_CONNECT_AFTERPREDIAL     = 2,
        MDMSTATUS_MS_NODIALTONE            = -2,
        MDMSTATUS_MS_NOCARRIER             = -3,
        MDMSTATUS_MS_BUSY                  = -4,
        MDMSTATUS_MS_ERROR                 = -5,
        MDMSTATUS_MS_NOANSWER              = -6,
        MDMSTATUS_MS_NOERRPROTOCOL         = -7,
        MDMSTATUS_MS_RING                  = -8,
        MDMSTATUS_NOPREDIAL                = -11,
        MDMSTATUS_ASYNS_ERR                = -20, /**<Asynchronous status error*/
        MDMSTATUS_SDLCS_ERR                = -50, /**<Synchronous status error*/
        MDMSTATUS_SDLCS_ERR_GETSNRMTIMEOUT = -51, /**<Receive SNMP frame timeout*/
        MDMSTATUS_SDLCS_ERR_GETRRTIMEOUT   = -52, /**<Receive 3 RR0 frames timeout*/
        MDMSTATUS_SDLCS_ERR_RECSIGNAL      = -53, /**<Signal interference*/
        MDMSTATUS_SDLCS_ERR_RECSNRMMORE    = -54, /**<Receive duplicate SNRM frame*/
} EM_MDMSTATUS;

/**
*@brief  Modem patch types
*/
typedef enum MDM_Patchtype {
        MDM_PatchType5 = 5, /**<Default type*/
        MDM_PatchType4 = 4, /**<Special use*/
        MDM_PatchType3 = 3, /**<Fast link*/
        MDM_PatchType2 = 2,
        MDM_PatchType1 = 1,
        MDM_PatchType0 = 0,
} EM_MDM_PatchType;

/**
 *@brief Modem dial types
*/
typedef enum {
        MODEM_DIAL_TYPE_NOT = 0,
        MODEM_DIAL_TYPE_SDLC, /**<Synchronous*/
        MODEM_DIAL_TYPE_ASYN, /**<Asynchronous*/
} EM_MODEM_DIAL_TYPE;

/**
 *@brief Modem baud rate in sync mode
 */
typedef enum {
        EM_MDM_BPS_1200 = 1, /**<1200BPS*/
        EM_MDM_BPS_2400,     /**<2400BPS*/
        EM_MDM_BPS_9600,     /**<9600BPS*/
}EM_MDM_BPS;


/**
 *@brief	Initialize synchronous dial
 *@param[in] emType	 Patch type
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
 *@li	\ref NDK_ERR_MODEM_GETVERFAIL "NDK_ERR_MODEM_GETVERFAIL" 	Failed to get the chip version
 *@li	\ref NDK_ERR_MODEM_ASYNINITFAIL	"NDK_ERR_MODEM_ASYNINITFAIL"	Failed to initialize

*/
int NDK_MdmSdlcInit(EM_MDM_PatchType emType);

/**
 *@brief    Initialize synchronous dial (Can work only once)
 *@param[in]    nType	 Patch type
 *@param[in]    bps      Baud rate
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
 *@li	\ref NDK_ERR_MODEM_GETVERFAIL "NDK_ERR_MODEM_GETVERFAIL" 	Failed to get the chip version
 *@li	\ref NDK_ERR_MODEM_ASYNINITFAIL	"NDK_ERR_MODEM_ASYNINITFAIL"	Failed to initialize
 */
int NDK_MdmSdlcSingleForceInit(EM_MDM_PatchType nType, EM_MDM_BPS bps);

/**
 *@brief    Get actual baud rate when connected
 *@param[out]    bps         Actual baud rate
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 	 Not dialing
 */
int NDK_MdmGetBps(EM_MDM_BPS *bps);

/**
 *@brief	 Initialize asynchronous dial
 *@param[in] emType 	Patch type
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_MODEM_GETVERFAIL "NDK_ERR_MODEM_GETVERFAIL"	Failed to get the chip version
 *@li	\ref NDK_ERR_MODEM_ASYNINITFAIL	"NDK_ERR_MODEM_ASYNINITFAIL"	Failed to initialize
*/
int NDK_MdmAsynInit(EM_MDM_PatchType emType);

/**
 *@brief	Dial
 *@param[in] pszDailNum 	Dial number
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"		 Invalid parameter
 *@li	\ref NDK_ERR_MODEM_NOLINE "NDK_ERR_MODEM_NOLINE" Cable not connected
 *@li	\ref NDK_ERR_MODEM_OTHERMACHINE "NDK_ERR_MODEM_OTHERMACHINE"		Collision detected
 *@li	\ref NDK_ERR_MODEM_SDLCDIALFAIL "NDK_ERR_MODEM_SDLCDIALFAIL" 	Failed to dial asynchronously
 *@li	\ref NDK_ERR_MODEM_ASYNDIALFAIL "NDK_ERR_MODEM_ASYNDIALFAIL" 	Failed to dial synchronously
 *@li	\ref NDK_ERR_MODEM_INIT_NOT "NDK_ERR_MODEM_INIT_NOT" 	Not initialized
*/
int NDK_MdmDial(const char * pszDailNum);

/**
 *@brief	 Check modem state.
 *@param[in] *pemStatus 	Modem state
 *@return
 *@li	\ref NDK_OK "NDK_OK"  		Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	 Invalid parameter
 *@li	\ref NDK_ERR_MODEM_INIT_NOT "NDK_ERR_MODEM_INIT_NOT" Not initialized
 *@li	\ref NDK_ERR_MODEM_GETSTATUSFAIL "NDK_ERR_MODEM_GETSTATUSFAIL" Failed to get status
*/
int NDK_MdmCheck(EM_MDMSTATUS  *pemStatus);

/**
 *@brief	Send data.
 *@param[in] pszData 	Data to be sent
 *@param[in] unDataLen 	Length of data sent
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	 Invalid parameter
 *@li	\ref NDK_ERR_MODEM_INIT_NOT "NDK_ERR_MODEM_INIT_NOT" 	Not initialized
 *@li	\ref NDK_ERR_MODEM_NOPREDIAL "NDK_ERR_MODEM_NOPREDIAL" 	No dial
 *@li	 \ref NDK_ERR_MODEM_SDLCWRITEFAIL	"NDK_ERR_MODEM_SDLCWRITEFAIL" 	Failed to send synchronously
 *@li	 \ref NDK_ERR_MODEM_ASYNWRITEFAIL  "NDK_ERR_MODEM_ASYNWRITEFAIL"    Failed to send asynchronously
 *@li	 \ref NDK_ERR_MODEM_INIT_NOT   "NDK_ERR_MODEM_INIT_NOT"              Not initialized
*/
int NDK_MdmWrite(const char *pszData, uint unDataLen);

/**
 *@brief	Receive data.
 *@param[in] *pszData 	Buffer to save data
 *@param[in] *punDataLen 	Length of data received
 *@param[in] unSenconds	Timeout in seconds
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
 *@li	\ref NDK_ERR_MODEM_INIT_NOT "NDK_ERR_MODEM_INIT_NOT"		Not initialized
 *@li	\ref NDK_ERR_TIMEOUT "NDK_ERR_TIMEOUT" 	 Timeout error
*/
int NDK_MdmRead(char  *pszData, uint *punDataLen, uint unSenconds);

/**
 *@brief	Hangup
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_MODEM_SDLCHANGUPFAIL "NDK_ERR_MODEM_SDLCHANGUPFAIL"
 *@li	\ref NDK_ERR_MODEM_ASYNHANGUPFAIL "NDK_ERR_MODEM_ASYNHANGUPFAIL"		Failed to hang up
 *@li	\ref NDK_ERR_MODEM_INIT_NOT "NDK_ERR_MODEM_INIT_NOT"			Not initialized
*/
int NDK_MdmHangup(void);

/**
 *@brief	 Clear modem buffer.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_MODEM_SDLCCLRBUFFAIL "NDK_ERR_MODEM_SDLCCLRBUFFAIL" 	Failed to clear buffer in synchronous mode
 *@li	\ref NDK_ERR_MODEM_ASYNCLRBUFFAIL "NDK_ERR_MODEM_ASYNCLRBUFFAIL" 		Failed to clear buffer in asynchronous mode
 *@li	\ref NDK_ERR_MODEM_INIT_NOT "NDK_ERR_MODEM_INIT_NOT" 		Not initialized
*/
int NDK_MdmClrbuf(void);

/**
 *@brief	Check if there is any data in modem buffer.
 *@param[in] *punReadLen 	Data length
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		 Invalid parameter
 *@li	\ref NDK_ERR_MODEM_GETBUFFLENFAIL "NDK_ERR_MODEM_GETBUFFLENFAIL"		Failed
 *@li	\ref NDK_ERR_MODEM_INIT_NOT "NDK_ERR_MODEM_INIT_NOT"	 	Not initialized
*/
int NDK_MdmGetreadlen(uint *punReadLen);

/**
 *@brief	Reset
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_MODEM_RESETFAIL "NDK_ERR_MODEM_RESETFAIL"		Failed
*/
int NDK_MdmReset(void);

/**
 *@brief	Execute AT command.
 *@param[in] psCmdStr 	Input command
 *@param[in] pszRespData 	Response data
 *@param[in] punLen 	Length of response data
 *@param[in] unTimeout  Timout in seconds
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	Invalid parameter
 *@li	\ref NDK_ERR_MODEM_ATCOMNORESPONSE "NDK_ERR_MODEM_ATCOMNORESPONSE" 		AT command error
*/
int NDK_MdmExCommand(uchar *psCmdStr, uchar *pszRespData, uint *punLen, uint unTimeout);

/**
 *@brief    Do auto-negotiation.
 *@param[in]   emModemDialType   Dial type
 *@param[in]   pszDialNum     Dial number
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	Invalid parameter
 *@li	\ref NDK_ERR_MODEM_SELFADAPTFAIL  " NDK_ERR_MODEM_SELFADAPTFAIL" 	Failed
*/
int NDK_MdmAdapt(EM_MODEM_DIAL_TYPE emModemDialType, const char *pszDialNum);

/** @}*/ // End of cable modem

/** @addtogroup Wireless
* @{
*/

/**
 *@brief  Commonly used AT commands
*/
typedef enum {
        WLM_CMD_E0     = 0,    /**<Echo off*/
        WLM_CMD_CSQ    = 1,    /**<Get signal strength*/
        WLM_CMD_CREG   = 2,    /**<Get network registration state*/
        WLM_CMD_CPIN   = 3,    /**<Query PIN*/
        WLM_CMD_CPIN0  = 4,    /**<Set PIN*/
        WLM_CMD_CGATT0 = 5,    /**<Logoff GPRS network*/
        WLM_CMD_CGATT1 = 6,    /**<Register GPRS network*/
        WLM_CMD_DIAL   = 7,    /**<Dial*/
        WLM_CMD_D2     = 8,    /**<Enable hangup by HW*/
        WLM_CMD_COPS   = 9,    /**<Register network*/
        WLM_CMD_CGMR   = 10,   /**<Get module version*/
        WLM_CMD_CGSN, /**<Get GSN No. (IMEI or MEID)*/
        WLM_CMD_CCID, /**<Get SIM card serial number*/
        WLM_CMD_END,  /**<End of command list*/
        WLM_CMD_UNDEFINE = 1024,
        WLM_CMD_PUBLIC   = 255
} EM_WLM_CMD;

/**
 *@brief  AT command
*/
typedef struct NDK_WL_ATCMD {
        EM_WLM_CMD AtCmdNo; /**<Command No.*/
        char *pcAtCmd;      /**<Command string*/
        char *pcAddParam;   /**<Additional parameters*/
} ST_ATCMD_PACK;

/**
 *@brief  Connetion status
*/
typedef enum {
        WLM_STATUS_UNTYPED    = 4,
        WLM_STATUS_NO_CARRIER = 3,    /**<No carrier*/
        WLM_STATUS_RING       = 2,    /**<Ring*/
        WLM_STATUS_CONNECT    = 1,    /**<Connect*/
        WLM_STATUS_OK         = 0,    /**<Success*/
        WLM_STATUS_ERROR      = -1,   /**<Error*/
        WLM_STATUS_RET_ERROR  = -114, /**<Error*/
} EM_WLM_STATUS;

typedef enum {
        WLM_INFO_CCID     = 0,
        WLM_INFO_IMSI     = 1,
        WLM_INFO_IMEI     = 2,
        WLM_INFO_UNDEFINE = 1024,
} EM_WLM_TYPE_INFO;

/**
 *@brief   Base station information
*/
typedef struct station_info {
        uint unMCC; /**<Country code*/
        uint unMNC; /**<Network code*/
        uint unLac; /**<Area code*/
        uint unCi;  /**<Cell ID*/
        int ndbm;   /**<Signal strength*/
} ST_STATION_INFO;

typedef struct {
        ST_STATION_INFO stMainStation;        /**<Main base station attached*/
        ST_STATION_INFO stNeighborStation[6]; /**<Nearby base stations*/
} ST_GPRS_STATION_INFO;

typedef struct {
        uint unMCC;       /**<Country code*/
        uint unMNC;       /**<Network code*/
        uint unBandClass; /**<Band class*/
        uint unChannel;   /**<Channel*/
        uint unBid;       /**<System  identification number*/
        uint unSid;       /**<Network identification number*/
        uint unNid;       /**<Area in network*/
} ST_CDMA_STATION_INFO;

typedef union {
        ST_CDMA_STATION_INFO CDMA_STATION; /**<CDMA base station*/
        ST_GPRS_STATION_INFO GPRS_STATION; /**<GPRS base station*/
} MODULE_UNION_INFO;

typedef enum {
        MODULE_TYPE_GPRS,    /**<GPRS module*/
        MODULE_TYPE_CDMA,    /**<CDMA  module*/
        MODULE_TYPE_WCDMA,   /**<WCDMA  module*/
        MODULE_TYPE_TDSCDMA, /**<TDSCDMA  module*/
} EM_MODULE_TYPE;

typedef struct {
        EM_MODULE_TYPE emModuleType;         /**<Module type*/
        MODULE_UNION_INFO ModuleStationInfo; /**<Base station information*/
} ST_MOBILE_STATION_INFO;

typedef enum  {
    SIM_SLOT1,          /**<Default after power on*/
    SIM_SLOT2,
    SIM_SLOT_UNKNOW,
} EM_WLM_SIM_SLOT;

/**
 *@brief	 Hardware reset.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		Failed to call driver function
*/
int NDK_WlModemReset(void);

/**
 *@brief	Close wireless modem.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		Failed to call driver function
*/
int NDK_WlModemClose(void);

/**
 *@brief 	Initialize wirelss modem
 *@param[in] nTimeout	Timeout in milliseconds
 *@param[in] pszPinPassWord	PIN
 *@param[out]	pemStatus	Wireless modem status
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
 *@li	\ref NDK_ERR_OVERFLOW "NDK_ERR_OVERFLOW" 	 Buffer overflow
 *@li	\ref NDK_ERR "NDK_ERR" 			Fail
 *@li	\ref NDK_ERR_TIMEOUT "NDK_ERR_TIMEOUT" 	 Timeout error
 *@li	\ref NDK_ERR_PIN_LOCKED "NDK_ERR_PIN_LOCKED" 	SIM card locked
 *@li	\ref NDK_ERR_PIN "NDK_ERR_PIN" 	Wrong PIN
 *@li	\ref NDK_ERR_PIN_UNDEFINE "NDK_ERR_PIN_UNDEFINE" 	Unknown error of SIM card
 *@li	\ref NDK_ERR_NO_SIMCARD "NDK_ERR_NO_SIMCARD" 	 No SIM card
 *@li	\ref NDK_ERR_WLM_SEND_AT_FAIL "NDK_ERR_WLM_SEND_AT_FAIL" 	  AT command error
*/
int NDK_WlInit(int nTimeout, const char *pszPinPassWord, EM_WLM_STATUS *pemStatus);

/**
 *@brief	 Get signal strength
 *@param[out]	pnSq	0-31:Signal strength; 99: Unknown; -1: Failed
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 			Fail
*/
int NDK_WlModemGetSQ(int *pnSq);

/**
 *@brief	Send AT command
 *@param[in] pstATCmdPack	AT command
 *@param[in] unMaxLen	Maximum buffer length of pszOutput
 *@param[in] unTimeout	Timeout in milliseconds
 *@param[out]	pszOutput	Output string
 *@param[out]	pemStatus	Wireless modem status
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	Failed to open device
 *@li	\ref NDK_ERR "NDK_ERR" 				   Fail
 *@li	\ref NDK_ERR_EMPTY "NDK_ERR_EMPTY" 			Empty string returned
 *@li	\ref NDK_ERR_OVERFLOW "NDK_ERR_OVERFLOW" 	Buffer overflow
*/
int NDK_WlSendATCmd(const ST_ATCMD_PACK *pstATCmdPack, char *pszOutput, uint unMaxLen, uint unTimeout, EM_WLM_STATUS *pemStatus);

/**
 *@brief 		Get CCID, IMSI, IMEI
 *@param[in] 	emType		CCID, IMSI, IMEI
 *@param[in] 	pszValue	Value of CCID, IMSI, IMEI
 *@param[in] 	unBufLen	    No less than 21.
 *@return
 *@li	 \ref NDK_OK "NDK_OK"  				Success
 *@li  \ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
 *@li  \ref NDK_ERR "NDK_ERR" 	      Fail
*/
int NDK_WlGetInfo(EM_WLM_TYPE_INFO emType, char *pszValue, uint unBufLen);

/**
 *@brief	Get base station information.

 *@param[out]	pstStationInfo	 Base station information
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
 *@li \ref NDK_ERR_NOT_SUPPORT "NDK_ERR_NOT_SUPPORT"	Not supported
*/
int NDK_WlGetStationInfo(ST_MOBILE_STATION_INFO * pstStationInfo);

/**
 *@brief	Close RF (Not implemented)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  					Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"	Fail
*/
int NDK_WlCloseRF(void);

/**
 *@brief	 Select SIM card (Not implemented)
 *@param[in] ucSimNo	SIM card No.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_WlSelSIM(uchar ucSimNo);

/**
 *@brief      Get current SIM card slot
 *@param[out]    pSimSlot  SIM card slot
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR "NDK_ERR" 				Fail
*/
int NDK_WlGetSimSlot(EM_WLM_SIM_SLOT *pSimSlot );

/**
 *@brief	Set SIM Card slot
 *@param[in]	SimSlot	SIM card slot
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	Invalid parameter
 *@li	\ref NDK_ERR_NOT_SUPPORT "NDK_ERR_NOT_SUPPORT"        Not supported
 *@li	\ref NDK_ERR "NDK_ERR" 				Fail
*/
int NDK_WlSetSimSlot(EM_WLM_SIM_SLOT SimSlot);

/** @}*/ // End of wireless modem


/** @addtogroup Socket
* @{
*/

/**
 *@brief	 Open TCP channel
 *@param[out]	punFd	TCP channel handle
 *@return
 *@li	\ref NDK_OK "NDK_OK"  					Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		 Invalid parameter
 *@li	\ref NDK_ERR_TCP_ALLOC "NDK_ERR_TCP_ALLOC" 		Out of memory
 *@li	\ref NDK_ERR_TCP_PARAM "NDK_ERR_TCP_PARAM" 		 Invalid parameter
 *@li	\ref NDK_ERR_TCP_TIMEOUT "NDK_ERR_TCP_TIMEOUT" 		Timeout error
 *@li	\ref NDK_ERR_TCP_INVADDR "NDK_ERR_TCP_INVADDR" 		Invalid address
 *@li	\ref NDK_ERR_TCP_CONNECT "NDK_ERR_TCP_CONNECT" 		 No connection
 *@li	\ref NDK_ERR_TCP_PROTOCOL "NDK_ERR_TCP_PROTOCOL" 	Protocol error
 *@li	\ref NDK_ERR_TCP_NETWORK "NDK_ERR_TCP_NETWORK" 		 Network error
*/
int NDK_TcpOpen(uint *punFd);

/**
 *@brief	Close TCP channel
 *@param[in] unFd	TCP channel handle
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail

*/
int NDK_TcpClose(uint unFd);

/**
 *@brief	 Wait for TCP to be closed
 *@details  Call this function after \ref NDK_TcpClose "NDK_TcpClose" to ensure TCP link is closed completely
 *@param[in] unFd	TCP channel handle
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail

*/
int NDK_TcpWait(uint unFd);

/**
 *@brief	Bind IP address and port
 *@param[in] unFd		TCP channel handle
 *@param[in] pszMyIp	IP address
 *@param[in] usMyPort	Port
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	Invalid parameter
 *@li	\ref NDK_ERR_TCP_PARAM "NDK_ERR_TCP_PARAM" 	 Invalid parameter
 *@li	\ref NDK_ERR_TCP_ALLOC "NDK_ERR_TCP_ALLOC" 		Out of memory
 *@li	\ref NDK_ERR_TCP_TIMEOUT "NDK_ERR_TCP_TIMEOUT" 		timeout error
 *@li	\ref NDK_ERR_TCP_INVADDR "NDK_ERR_TCP_INVADDR" 		Invalid address
 *@li	\ref NDK_ERR_TCP_CONNECT "NDK_ERR_TCP_CONNECT" 		No connection
 *@li	\ref NDK_ERR_TCP_PROTOCOL "NDK_ERR_TCP_PROTOCOL" 		Protocol error
 *@li	\ref NDK_ERR_TCP_NETWORK "NDK_ERR_TCP_NETWORK" 			Network error
*/
int NDK_TcpBind(uint unFd, const char *pszMyIp, ushort usMyPort);

/**
 *@brief	Connect
 *@param[in] unFd	TCP channel handle
 *@param[in] pszRemoteIp	Remote address
 *@param[in] usRemotePort	Remote port
 *@param[in] unTimeout	Timeout in seconds
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"  Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR"          	Fail
 *@li	\ref NDK_ERR_TCP_TIMEOUT "NDK_ERR_TCP_TIMEOUT"          Timeout error
 *@li	\ref NDK_ERR_ETH_PULLOUT "NDK_ERR_ETH_PULLOUT"          Ethernet cable not plugged
 *@li	\ref NDK_ERR_LINUX_TCP_TIMEOUT "NDK_ERR_LINUX_TCP_TIMEOUT"         Remote port error
 *@li	\ref NDK_ERR_LINUX_TCP_REFUSE "NDK_ERR_LINUX_TCP_REFUSE"          Remote port refused
 *@li	\ref NDK_ERR_LINUX_TCP_NOT_OPEN "NDK_ERR_LINUX_TCP_NOT_OPEN"       TCP channel not opened
*/
int NDK_TcpConnect(uint unFd, const char *pszRemoteIp, ushort usRemotePort, uint unTimeout);

/**
 *@brief	 Send data
 *@param[in] unFd	TCP channel handle
 *@param[in] pvInbuf	Send buffer
 *@param[in] unLen	Length of data sent
 *@param[in] unTimeout	Timeout in seconds
 *@param[out]	punWriteLen	Actual length sent
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_TCP_SEND "NDK_ERR_TCP_SEND" 	Fail
*/
int NDK_TcpWrite(uint unFd, const void *pvInbuf, uint unLen, uint unTimeout, uint *punWriteLen);

/**
 *@brief	 Receive data
 *@param[in] unFd	TCP channel handle
 *@param[in] unLen	Length of data to be received
 *@param[in] unTimeout	Timeout in seconds
 *@param[out]	pvOutBuf	Buffer to save data
 *@param[out]	punReadLen	 Actual length sent
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 			Fail
 *@li	\ref NDK_ERR_TCP_TIMEOUT "NDK_ERR_TCP_TIMEOUT" 			Timeout error
 *@li	\ref NDK_ERR_TCP_RECV "NDK_ERR_TCP_RECV" 			Fail
*/
int NDK_TcpRead(uint unFd, void *pvOutBuf, uint unLen, uint unTimeout, uint *punReadLen);

/**
 *@brief     Shut down TCP channel
 *@param[in]   unFd   TCP channel handle
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR "NDK_ERR" 		Fail
*/
int NDK_TcpReset(uint unFd);

/** @}*/ // End of socket

/** @addtogroup PPP
* @{
*/

/**
 *@brief  For long link only
*/
#define STATUS_OPEN  0x03
#define STATUS_CLOSE 0x02
#define LCP_ECHO_OPEN  STATUS_OPEN         /**<ECHO state*/
#define LCP_ECHO_CLOSE STATUS_CLOSE
#define LCP_DCD_OPEN     (STATUS_OPEN<<2)  /**<DCD state*/
#define LCP_DCD_CLOSE    (STATUS_CLOSE<<2)
#define LCP_PPP_KEEP     (STATUS_OPEN<<4)  /**<PPP long link state*/
#define LCP_PPP_UNKEEPED (STATUS_CLOSE<<4)

/**
 *@brief  PPP connection state
*/
typedef enum {
        PPP_STATUS_DISCONNECT    = 0, /**<Not connected*/
        PPP_STATUS_CONNECTING    = 2, /**<Connecting*/
        PPP_STATUS_CONNECTED     = 5, /**<Connected*/
        PPP_STATUS_DISCONNECTING = 6  /**<Hanging up*/
} EM_PPP_STATUS;

/**
 *@brief  PPP configuration
*/
typedef struct {
        int nDevType;                    /**<0: Wireless; 1: Wired*/
        unsigned int nPPPFlag;           /**<Indicates if long link is supported*/
        char szApn[64];                  /**<APN*/
        char szDailNum[32];              /**<Dial number*/
        int (*ModemDial)(void);          /**<Dial function*/
        unsigned int PPPIntervalTimeOut; /**<Time interval of sending data package for long link (No less than 30 seconds)*/
        unsigned char nMinSQVal;         /**<Minimum signal strength allowed during initialization*/
        char szPin[31];                  /**<SIM card PIN*/
        char nPPPHostIP[20];             /**<Host IP that needs to ping for long link*/
} ST_PPP_CFG;

/**
 *@brief	 Configure PPP
 *@param[in] pstPPPCfg	PPP parameter
 *@param[in] unValidLen	Length of PPP parameter
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
 *@li	\ref NDK_ERR_PPP_DEVICE "NDK_ERR_PPP_DEVICE" 		Invalid PPP device
 *@li	\ref NDK_ERR_SHM "NDK_ERR_SHM" 		   Failed to share memory
 *@li	\ref NDK_ERR_PPP_PARAM "NDK_ERR_PPP_PARAM" 		Invalid parameter
*/
int NDK_PppSetCfg(ST_PPP_CFG *pstPPPCfg, uint unValidLen);

/**
 *@brief	 Dail
 *@param[in] pszUserName	Username
 *@param[in] pszPassword	Password
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"  Invalid parameter
 *@li	\ref NDK_ERR_PPP_OPEN "NDK_ERR_PPP_OPEN" 	PPP not opened
 *@li	\ref NDK_ERR_SHM "NDK_ERR_SHM" 	  Failed to share memory
 *@li	\ref NDK_ERR_PPP_DEVICE "NDK_ERR_PPP_DEVICE" 		Invalid PPP device
*/
int NDK_PppDial(const char *pszUserName, const char *pszPassword);

/**
 *@brief	 Hang up
 *@param[in] nHangupType	0: Non-block; 1: Block
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		 Invalid parameter
 *@li	\ref NDK_ERR_TIMEOUT "NDK_ERR_TIMEOUT" 		Timeout error
 *@li	\ref NDK_ERR_SHM "NDK_ERR_SHM" 		 Failed to share memory
*/
int NDK_PppHangup(int nHangupType);

/**
 *@brief	 Get PPP status
 *@param[out]	pemStatus	PPP status
 *@param[out]	pnErrCode	Error code
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	       Invalid parameter
 *@li	\ref NDK_ERR_SHM "NDK_ERR_SHM" 				Failed to share memory
*/
int NDK_PppCheck(EM_PPP_STATUS *pemStatus, int *pnErrCode);

/**
 *@brief	Get local and host address
 *@param[out]	pulLocalAddr	Local address
 *@param[out]	pulHostAddr	Host address
 *@return
 *@li	\ref NDK_OK "NDK_OK"  		 Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 		 Fail
*/
int NDK_PppGetAddr(ulong *pulLocalAddr, ulong *pulHostAddr);

/**
 *@brief	 Convert IP in dotted decimal format to long integer
 *@param[in] pszIp	IP address string
 *@param[out]	pulIpAddr	Long integer number
 *@return
 *@li	\ref NDK_OK "NDK_OK"  			 Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 		  Fail
*/
int NDK_PppAddrChange(register const char *pszIp, ulong *pulIpAddr);

/** @}*/ // End of PPP

/** @addtogroup Ethernet
* @{
*/

/**
 *@brief  Communication types
*/
typedef enum {
        COMM_TYPE_ETH,    /**<Ethernet*/
        COMM_TYPE_WIFI,   /**<WiFi*/
        COMM_TYPE_PPP,    /**<PPP*/
        COMM_TYPE_BTH,    /**<Bluetooth*/
        COMM_TYPE_UNKNOW,
} EM_COMM_TYPE;

/**
 *@brief 	Set network address (Only IPV4 supported)
 *@param[in] pszIp	Local IP address string
 *@param[in] pszMask  Local subnet mask string
 *@param[in] pszGateway	 Local gateway address string
 *@param[in] 	pszDns	DNS server address
 *@return
 *@li	\ref NDK_OK "NDK_OK"  		Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	Failed to open device
 *@li	\ref NDK_ERR "NDK_ERR" 	 Fail
 *@li	\ref NDK_ERR_NET_ADDRILLEGAL "NDK_ERR_NET_ADDRILLEGAL"  Invalid address format
*/
int NDK_EthSetAddress(const char *pszIp, const char *pszMask, const char *pszGateway, const char *pszDns);

/**
 *@brief	 Get MAC address
 *@param[out]	pszMacAddr	MAC address
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 			  Fail
*/
int NDK_EthGetMacAddr(char *pszMacAddr);

/**
 *@brief	 Get network address (Only IPV4 supported)
 *@param[out]	pszIp	IP address
 *@param[out]	pszGateway	Gateway address
 *@param[out]	pszMask	Subnet mask
 *@param[out]	pszDns	DNS server address
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_NET_GETADDR "NDK_ERR_NET_GETADDR" 		Fail
 *@li	\ref NDK_ERR "NDK_ERR" 		  Success but failed to get DNS server address
*/
int NDK_EthGetNetAddr(char *pszIp, char *pszMask, char *pszGateway, char *pszDns);

/**
 *@brief	 Get network address using DHCP

 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR "NDK_ERR" 		Fail
 *@li	\ref NDK_ERR_NET_GETADDR "NDK_ERR_NET_GETADDR" 			Failed to get IP address or subnet mask or gateway
 *@li	\ref NDK_ERR_NET_ADDRILLEGAL "NDK_ERR_NET_ADDRILLEGAL" 	 Invalid address format
 *@li	\ref NDK_ERR_NET_GATEWAY "NDK_ERR_NET_GATEWAY" 	     Failed to get DNS
*/
int NDK_NetDHCP(void);

/**
 *@brief    Perform DNS resolution
 *@param[in]    pszDnsIp    IP address needs to be resolved
 *@param[in]    pszDnsName   DNS name
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li \ref NDK_ERR_PARA "NDK_ERR_PARA"     Invalid parameter
 *@li \ref NDK_ERR "NDK_ERR"          Fail
*/
int NDK_GetDnsIp(char *pszDnsIp, const char *pszDnsName);

/**
   *@brief    Disconnect
   *@details   This function will also delete route information.
   *@return
   *@li	\ref NDK_OK "NDK_OK"  				Success
   *@li   \ref NDK_ERR "NDK_ERR"          Fail
*/
int NDK_EthDisConnect(void);

/**
 *@brief Add route
 *@param[in]    emCommType     Communication type
 *@param[in]    pszDestIP      Server IP
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li \ref NDK_ERR_PARA "NDK_ERR_PARA"       Invalid parameter
 *@li \ref NDK_ERR_NET_UNKNOWN_COMMTYPE "NDK_ERR_NET_UNKNOWN_COMMTYPE"              Invalid communication type
 *@li \ref NDK_ERR_NET_INVALIDIPSTR "NDK_ERR_NET_INVALIDIPSTR"                       Invalid IP string
 *@li \ref NDK_ERR_NET_UNSUPPORT_COMMTYPE "NDK_ERR_NET_UNSUPPORT_COMMTYPE"    Communication type not supported
*/
int NDK_NetAddRouterTable(EM_COMM_TYPE emCommType, char *pszDestIP);

/**
 *@brief  Perform DNS resolution for different communication types
 *@param[in]     emCommType     Communication type
 *@param[in]     pszDnsIp       IP address resolved
 *@param[in]     pszDnsName     DNS name
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li \ref NDK_ERR "NDK_ERR"     Failed
 *@li \ref NDK_ERR_PARA "NDK_ERR_PARA"        Invalid parameter
 *@li \ref NDK_ERR_NET_UNKNOWN_COMMTYPE "NDK_ERR_NET_UNKNOWN_COMMTYPE"             Invalid communication type
*/
int NDK_NetDnsResolv(EM_COMM_TYPE emCommType, char *pszDnsIp, char *pszDnsName);

/**
 *@brief    Set DNS server
 *@param[in]  emCommType 	Communication type
 *@param[in]  pszDns		       Up to 3 DNS server addresses separated by ";"
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li \ref NDK_ERR "NDK_ERR"     Failed to set DNS
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"		Invalid parameter
 *@li	\ref NDK_ERR_NET_UNKNOWN_COMMTYPE "NDK_ERR_NET_UNKNOWN_COMMTYPE"				Invalid communication type
 *@li	\ref NDK_ERR_NET_ADDRILLEGAL "NDK_ERR_NET_ADDRILLEGAL"	Invalid address format
*/
int NDK_NetSetDns(EM_COMM_TYPE emCommType, const char *pszDns);

/**
 *@brief    Get network address for different communication types.
 *@param[in]    emComtype   Communication type
 *@param[out]   pszIp    IP address
 *@param[out]   pszGateway  Gateway address
 *@param[out]   pszMask     Subnet mask
 *@param[out]   pszDns     DNS server IP address
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li \ref NDK_ERR_PARA "NDK_ERR_PARA"      Invalid parameter
 *@li \ref NDK_ERR_NET_GETADDR "NDK_ERR_NET_GETADDR"      Failed to get IP address /subnet mask /gateway
 *@li \ref NDK_ERR "NDK_ERR"      Failed to get DNS
*/
int NDK_NetGetAddr(EM_COMM_TYPE emComtype, char *pszIp, char *pszMask, char *pszGateway, char *pszDns);

/**
 *@brief    Perform ping (Only IPV4 supported)
 *@param[in]   pszIp   IP address string to ping
 *@param[in]   nTimeoutMs  Timeout in milliseconds
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 				Fail
*/
int NDK_NetPing(char *pszIp, uint nTimeoutMs);

/** @}*/ // End of ethernet

/** @addtogroup MagneticCard
* @{
*/

/**
 *@brief	Open magnetic card reader
 *@return
 *@li	\ref NDK_OK "NDK_OK"  					Success
 *@li	\ref NDK_ERR "NDK_ERR" 	  Fail
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
*/
int NDK_MagOpen(void);

/**
 *@brief Close magnetic card reader
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_MagClose(void);

/**
 *@brief	 Reset magnetic card reader
 *@details	 Reset magnetic head and clear buffer
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR "NDK_ERR" 		 Fail
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 	 Failed to call driver function
*/
int NDK_MagReset(void);

/**
 *@brief	 Check if card swiped
 *@param[out]	psSwiped	1: Swipei; 0: Not swiped
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 				Fail
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 			 Failed to call driver function
*/
int NDK_MagSwiped(uchar * psSwiped);

/**
 *@brief	 Read data of track 1, 2, 3
 *@details	Work with \ref NDK_MagSwiped "NDK_MagSwiped"
 *@param[out]	pszTk1	Buffer to save track 1 data
 *@param[out]	pszTk2	Buffer to save track 2 data
 *@param[out]	pszTk3	Buffer to save track 3 data
 *@param[out]	pnErrorCode	Error code
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR "NDK_ERR" 				Fail
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 			 Failed to call driver function
 *@li	\ref NDK_ERR_NOSWIPED "NDK_ERR_NOSWIPED" 		No card swiped
*/
int NDK_MagReadNormal(char *pszTk1, char *pszTk2, char *pszTk3, int *pnErrorCode);

/**
 *@brief	Read raw data of track 1, 2, 3
 *@details	Work with \ref NDK_MagSwiped "NDK_MagSwiped"
 *@param[out]	pszTk1	Buffer to save track 1 raw data
 *@param[out]	pusTk1Len	Track 1 data length
 *@param[out]	pszTk2	Buffer to save track 2 raw data
 *@param[out]	pusTk2Len	Track 2 data 2 length
 *@param[out]	pszTk3	Buffer to save track 3 raw data
 *@param[out]	pusTk3Len	Track 3 data length
 *@return:
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 				Fail
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		Failed to call driver function
*/
int NDK_MagReadRaw(uchar *pszTk1, ushort* pusTk1Len, uchar *pszTk2, ushort* pusTk2Len, uchar *pszTk3, ushort* pusTk3Len );

/** @}*/ // End of magnetic card

/** @addtogroup SmartCard
* @{
*/

/**
 *@brief  Smart card or chip types
*/
typedef enum {
        ICTYPE_IC,      /**<Smart card*/
        ICTYPE_SAM1,    /**<SAM1 card*/
        ICTYPE_SAM2,    /**<SAM2 card*/
        ICTYPE_SAM3,    /**<SAM3 card*/
        ICTYPE_SAM4,    /**<SAM4 card*/
        ICTYPE_M_1,     /**<at24c01 at24c02 at24c04 at24c08 at24c16 at24c32 at24c64*/
        ICTYPE_M_2,     /**<sle44x2*/
        ICTYPE_M_3,     /**<sle44x8*/
        ICTYPE_M_4,     /**<at88sc102*/
        ICTYPE_M_5,     /**<at88sc1604*/
        ICTYPE_M_6,     /**<at88sc1608*/
        ICTYPE_ISO7816, /**<ISO7816 standard*/
        ICTYPE_M_7,     /**<at88sc153*/
        ICTYPE_M_1_1,   /**<at24c01*/
        ICTYPE_M_1_2,   /**<at24c02*/
        ICTYPE_M_1_4,   /**<at24c04*/
        ICTYPE_M_1_8,   /**<at24c08*/
        ICTYPE_M_1_16,  /**<at24c16*/
        ICTYPE_M_1_32,  /**<at24c32*/
        ICTYPE_M_1_64,  /**<at24c64*/
} EM_ICTYPE;

/**
 *@brief	 Get driver verison
 *@param[out] 	pszVersion  Driver version string (No less than 16 bytes)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  					Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 			Failed to call driver function
*/
int  NDK_IccGetVersion(char *pszVersion);

/**
 *@brief	 Set card type
 *@param[in] emIcType      Card type (\ref EM_ICTYPE "EM_ICTYPE")
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 			Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 	         	Failed to call driver function
*/
int NDK_IccSetType(EM_ICTYPE emIcType);

/**
 *@brief	 Get card type
 *@param[out] 	pemIcType Card type
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 			Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV")   	Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		Failed to call driver function
*/
int NDK_IccGetType(EM_ICTYPE *pemIcType);

/**
 *@brief	Detect card
 *@param[out] 	pnSta   Bit0(For smart card) 1: Inserted; 0: Not inserted\n
 *					Bit1(For smart card) 1: Powered up; 0: Not powered up\n
 *					Bit2: Reserved\n
 *					Bit3: Reserved\n
 *					Bit4(For SAM card 1) 1: Powered up; 0: Not powered up\n
 *					Bit5(For SAM card 2) 1: Powered up; 0: Not powered up\n
 *					Bit6(For SAM card 3) 1: Powered up; 0: Not powered up\n
 *					Bit7(For SAM card 4) 1: Powered up; 0: Not powered up
 *
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 			Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 			Failed to open device
*/
int NDK_IccDetect(int *pnSta);

/**
 *@brief 	Power up
 *@param[in] emIcType    Card type (\ref EM_ICTYPE "EM_ICTYPE")
 *@param[out]  psAtrBuf  	ATR data
 *@param[out]   pnAtrLen  	Length of ATR data
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 				Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 				Fail
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	Failed to open device
 *@li	\ref NDK_ERR_ICC_CARDNOREADY_ERR "NDK_ERR_ICC_CARDNOREADY_ERR" 	 Smart card not ready
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 	 Failed to call driver function
*/
int NDK_IccPowerUp (EM_ICTYPE emIcType, uchar *psAtrBuf, int *pnAtrLen);

/**
 *@brief	Power down
 *@param[in] emIcType Card type (\ref EM_ICTYPE "EM_ICTYPE")
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 			Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 	 Failed to call driver function
*/
int NDK_IccPowerDown(EM_ICTYPE emIcType);

/**
 *@brief	Send or Receive data
 *@param[in] emIcType	 Card type (\ref EM_ICTYPE "EM_ICTYPE")
 *@param[in] nSendLen	Length of data sent
 *@param[in] psSendBuf	Data sent
 *@param[out] 	pnRecvLen   Length of data received
 *@param[out] 	psRecvBuf   Data received
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 			Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 	 Failed to call driver function
*/
int NDK_Iccrw(EM_ICTYPE emIcType, int nSendLen,  uchar *psSendBuf, int *pnRecvLen,  uchar *psRecvBuf);

/** @}*/ // End of smart card

/** @addtogroup ContactlessCard
* @{
*/

/**
 *@brief  Contactless chip types
*/
typedef enum {
        RFID_Autoscan = 0x00,
        RFID_RC531 =    0x01,
        RFID_PN512 =    0x02,
        RFID_AS3911 =   0x03,
        RFID_No =       0xFF,
} EM_RFID;

/**
 *@brief	 Get driver version
 *@param[in] pszVersion	Driver version string
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		Failed to call driver function
*/
int  NDK_RfidVersion(uchar *pszVersion);

/**
 *@brief	 Initialize contactless card reader
 *@param[out] 	psStatus  Reserved
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		  	Failed to call driver function
 *@li	\ref NDK_ERR_RFID_INITSTA "NDK_ERR_RFID_INITSTA" 	Device fault or not configured
*/
int NDK_RfidInit(uchar *psStatus);

/**
 *@brief	Turn on contactless card reader
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR "NDK_ERR" 		Fail
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 	        	Failed to call driver function
*/
int NDK_RfidOpenRf(void);

/**
 *@brief	Turn off contactless card reader
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR "NDK_ERR" 		Fail
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 			Failed to call driver function
*/
int NDK_RfidCloseRf(void);

/**
 *@brief	Get card status
 *@return
 *@li	\ref NDK_OK "NDK_OK"  		Success (Powered and busy)
 *@li	\ref NDK_ERR_RFID_NOTACTIV "NDK_ERR_RFID_NOTACTIV		Fail  (Idle)
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 			Failed to call driver function
*/
int NDK_RfidPiccState(void);

/**
 *@brief	Enable/Disable timeout control
 *@param[in] ucTimeOutCtl 0: Disable; 1: Enable
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		Failed to call driver function
*/
int  NDK_RfidTimeOutCtl(uchar ucTimeOutCtl);

/**
 *@brief	 Suspend contactless card reader
 *@return
 *@li	\ref NDK_OK "NDK_OK"  		Success
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 			Failed to call driver function
*/
int NDK_RfidSuspend(void);

/**
 *@brief	Resume contactless card reader
 *@return
 *@li	\ref NDK_OK "NDK_OK"  		execution succeeded
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		      Failed to call driver function
*/
int NDK_RfidResume(void);

/**
 *@brief	Set card seeking strategy
 *@details Set it once before seeking card and set it to Type A for M1 card
 *@param[in] ucPiccType  0xc: TYPE A cards only, 0xcb: TYPE B cards only, 0xcd: Both Type A and TYPE B cards.
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
 *@li	\ref NDK_ERR_MI_NOTAGERR "NDK_ERR_MI_NOTAGERR" 	No card,		0xff
 *@li	\ref NDK_ERR_MI_CRCERR "NDK_ERR_MI_CRCERR" 	CRC error,        0xfe
 *@li	\ref NDK_ERR_MI_EMPTY "NDK_ERR_MI_EMPTY" 	Not empty,          0xfd
 *@li	\ref NDK_ERR_MI_AUTHERR "NDK_ERR_MI_AUTHERR" 	Authentication error,   0xfc
 *@li	\ref NDK_ERR_MI_PARITYERR "NDK_ERR_MI_PARITYERR" 	Parity error,    oxfb
 *@li	\ref NDK_ERR_MI_CODEERR "NDK_ERR_MI_CODEERR" 		Received code error, 0xfa
 *@li	\ref NDK_ERR_MI_SERNRERR "NDK_ERR_MI_SERNRERR" 	  Anti collision data check error, 0xf8
 *@li	\ref NDK_ERR_MI_KEYERR "NDK_ERR_MI_KEYERR" 	       Key error, 0xf7
 *@li	\ref NDK_ERR_MI_NOTAUTHERR "NDK_ERR_MI_NOTAUTHERR" 	 Not authenticated,  0xf6
 *@li	\ref NDK_ERR_MI_BITCOUNTERR "NDK_ERR_MI_BITCOUNTERR" 	 Received bit error,  0xf5
 *@li	\ref NDK_ERR_MI_BYTECOUNTERR "NDK_ERR_MI_BYTECOUNTERR" 	  Received byte error, 0xf4
 *@li	\ref NDK_ERR_MI_WriteFifo "NDK_ERR_MI_WriteFifo" 	   FIFO write error, 0xf3
 *@li	\ref NDK_ERR_MI_TRANSERR "NDK_ERR_MI_TRANSERR" 	       Transfer error, 0xf2
 *@li	\ref NDK_ERR_MI_WRITEERR "NDK_ERR_MI_WRITEERR" 	       Write error, 0xf1
 *@li	\ref NDK_ERR_MI_INCRERR "NDK_ERR_MI_INCRERR" 	       Increment error, 0xf0
 *@li	\ref NDK_ERR_MI_DECRERR "NDK_ERR_MI_DECRERR" 	        Decrement error,  0xef
 *@li	\ref NDK_ERR_MI_OVFLERR "NDK_ERR_MI_OVFLERR" 	       Overflow error, 0xed
 *@li	\ref NDK_ERR_MI_FRAMINGERR "NDK_ERR_MI_FRAMINGERR" 	   Framing error,  0xeb
 *@li	\ref NDK_ERR_MI_COLLERR "NDK_ERR_MI_COLLERR" 	       Collison detected,    0xe8
 *@li	\ref NDK_ERR_MI_INTERFACEERR "NDK_ERR_MI_INTERFACEERR"    Reset interface error,  0xe6
 *@li	\ref NDK_ERR_MI_ACCESSTIMEOUT "NDK_ERR_MI_ACCESSTIMEOUT" 	 Receive timeout,  0xe5
 *@li	\ref NDK_ERR_MI_PROTOCOLERR "NDK_ERR_MI_PROTOCOLERR" 	      Protocol error,  0xe4
 *@li	\ref NDK_ERR_MI_QUIT "NDK_ERR_MI_QUIT" 	                    Abnomal quit,   0xe2
 *@li	\ref NDK_ERR_MI_PPSErr "NDK_ERR_MI_PPSErr" 	             PPS error,  0xe1
 *@li	\ref NDK_ERR_MI_SpiRequest "NDK_ERR_MI_SpiRequest" 	       Request SPI error,  0xa0
 *@li	\ref NDK_ERR_MI_NY_IMPLEMENTED "NDK_ERR_MI_NY_IMPLEMENTED" 	   Unknown error, 0x9c
 *@li	\ref NDK_ERR_MI_CardTypeErr "NDK_ERR_MI_CardTypeErr" 	     Wrong card type, 0x83
 *@li	\ref NDK_ERR_MI_ParaErrInIoctl "NDK_ERR_MI_ParaErrInIoctl" 	 Invalid parameter for ioctl, 0x82
 *@li	\ref NDK_ERR_MI_Para "NDK_ERR_MI_Para"                    Invalid internal parameter, 0xa9
*/
int NDK_RfidPiccType(uchar ucPiccType);

/**
 *@brief	 Detect card
 *@param[out] psPiccType	0xcc: TYPE A card; 0xcb: TYPE B card
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
 *@li	\ref NDK_ERR_MI_NOTAGERR "NDK_ERR_MI_NOTAGERR" 	No card,		0xff
 *@li	\ref NDK_ERR_MI_CRCERR "NDK_ERR_MI_CRCERR" 	CRC error,        0xfe
 *@li	\ref NDK_ERR_MI_EMPTY "NDK_ERR_MI_EMPTY" 	Not empty,          0xfd
 *@li	\ref NDK_ERR_MI_AUTHERR "NDK_ERR_MI_AUTHERR" 	Authentication error,   0xfc
 *@li	\ref NDK_ERR_MI_PARITYERR "NDK_ERR_MI_PARITYERR" 	Parity error,    oxfb
 *@li	\ref NDK_ERR_MI_CODEERR "NDK_ERR_MI_CODEERR" 		Received code error, 0xfa
 *@li	\ref NDK_ERR_MI_SERNRERR "NDK_ERR_MI_SERNRERR" 	  Anti collision data check error, 0xf8
 *@li	\ref NDK_ERR_MI_KEYERR "NDK_ERR_MI_KEYERR" 	       Key error, 0xf7
 *@li	\ref NDK_ERR_MI_NOTAUTHERR "NDK_ERR_MI_NOTAUTHERR" 	 Not authenticated,  0xf6
 *@li	\ref NDK_ERR_MI_BITCOUNTERR "NDK_ERR_MI_BITCOUNTERR" 	 Received bit error,  0xf5
 *@li	\ref NDK_ERR_MI_BYTECOUNTERR "NDK_ERR_MI_BYTECOUNTERR" 	  Received byte error, 0xf4
 *@li	\ref NDK_ERR_MI_WriteFifo "NDK_ERR_MI_WriteFifo" 	   FIFO write error, 0xf3
 *@li	\ref NDK_ERR_MI_TRANSERR "NDK_ERR_MI_TRANSERR" 	       Transfer error, 0xf2
 *@li	\ref NDK_ERR_MI_WRITEERR "NDK_ERR_MI_WRITEERR" 	       Write error, 0xf1
 *@li	\ref NDK_ERR_MI_INCRERR "NDK_ERR_MI_INCRERR" 	       Increment error, 0xf0
 *@li	\ref NDK_ERR_MI_DECRERR "NDK_ERR_MI_DECRERR" 	        Decrement error,  0xef
 *@li	\ref NDK_ERR_MI_OVFLERR "NDK_ERR_MI_OVFLERR" 	       Overflow error, 0xed
 *@li	\ref NDK_ERR_MI_FRAMINGERR "NDK_ERR_MI_FRAMINGERR" 	   Framing error,  0xeb
 *@li	\ref NDK_ERR_MI_COLLERR "NDK_ERR_MI_COLLERR" 	       Collison detected,    0xe8
 *@li	\ref NDK_ERR_MI_INTERFACEERR "NDK_ERR_MI_INTERFACEERR"    Reset interface error,  0xe6
 *@li	\ref NDK_ERR_MI_ACCESSTIMEOUT "NDK_ERR_MI_ACCESSTIMEOUT" 	 Receive timeout,  0xe5
 *@li	\ref NDK_ERR_MI_PROTOCOLERR "NDK_ERR_MI_PROTOCOLERR" 	      Protocol error,  0xe4
 *@li	\ref NDK_ERR_MI_QUIT "NDK_ERR_MI_QUIT" 	                    Abnomal quit,   0xe2
 *@li	\ref NDK_ERR_MI_PPSErr "NDK_ERR_MI_PPSErr" 	             PPS error,  0xe1
 *@li	\ref NDK_ERR_MI_SpiRequest "NDK_ERR_MI_SpiRequest" 	       Request SPI error,  0xa0
 *@li	\ref NDK_ERR_MI_NY_IMPLEMENTED "NDK_ERR_MI_NY_IMPLEMENTED" 	   Unknown error, 0x9c
 *@li	\ref NDK_ERR_MI_CardTypeErr "NDK_ERR_MI_CardTypeErr" 	     Wrong card type, 0x83
 *@li	\ref NDK_ERR_MI_ParaErrInIoctl "NDK_ERR_MI_ParaErrInIoctl" 	 Invalid parameter for ioctl, 0x82
 *@li	\ref NDK_ERR_MI_Para "NDK_ERR_MI_Para"                    Invalid internal parameter, 0xa9
*/
int NDK_RfidPiccDetect(uchar *psPiccType);

/**
 *@brief	 Activate card
 *@param[out] psPiccType	0xcc: TYPE A card; 0xcb: TYPE B card
 *@param[out]	pnDataLen	Data length
 *@param[out]	psDataBuf	Data buffer(UID for Type A card, UID for Type B card in psDataBuf[1]~[4], application or protocol information in other bytes)
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
 *@li	\ref NDK_ERR_MI_NOTAGERR "NDK_ERR_MI_NOTAGERR" 	No card,		0xff
 *@li	\ref NDK_ERR_MI_CRCERR "NDK_ERR_MI_CRCERR" 	CRC error,        0xfe
 *@li	\ref NDK_ERR_MI_EMPTY "NDK_ERR_MI_EMPTY" 	Not empty,          0xfd
 *@li	\ref NDK_ERR_MI_AUTHERR "NDK_ERR_MI_AUTHERR" 	Authentication error,   0xfc
 *@li	\ref NDK_ERR_MI_PARITYERR "NDK_ERR_MI_PARITYERR" 	Parity error,    oxfb
 *@li	\ref NDK_ERR_MI_CODEERR "NDK_ERR_MI_CODEERR" 		Received code error, 0xfa
 *@li	\ref NDK_ERR_MI_SERNRERR "NDK_ERR_MI_SERNRERR" 	  Anti collision data check error, 0xf8
 *@li	\ref NDK_ERR_MI_KEYERR "NDK_ERR_MI_KEYERR" 	       Key error, 0xf7
 *@li	\ref NDK_ERR_MI_NOTAUTHERR "NDK_ERR_MI_NOTAUTHERR" 	 Not authenticated,  0xf6
 *@li	\ref NDK_ERR_MI_BITCOUNTERR "NDK_ERR_MI_BITCOUNTERR" 	 Received bit error,  0xf5
 *@li	\ref NDK_ERR_MI_BYTECOUNTERR "NDK_ERR_MI_BYTECOUNTERR" 	  Received byte error, 0xf4
 *@li	\ref NDK_ERR_MI_WriteFifo "NDK_ERR_MI_WriteFifo" 	   FIFO write error, 0xf3
 *@li	\ref NDK_ERR_MI_TRANSERR "NDK_ERR_MI_TRANSERR" 	       Transfer error, 0xf2
 *@li	\ref NDK_ERR_MI_WRITEERR "NDK_ERR_MI_WRITEERR" 	       Write error, 0xf1
 *@li	\ref NDK_ERR_MI_INCRERR "NDK_ERR_MI_INCRERR" 	       Increment error, 0xf0
 *@li	\ref NDK_ERR_MI_DECRERR "NDK_ERR_MI_DECRERR" 	        Decrement error,  0xef
 *@li	\ref NDK_ERR_MI_OVFLERR "NDK_ERR_MI_OVFLERR" 	       Overflow error, 0xed
 *@li	\ref NDK_ERR_MI_FRAMINGERR "NDK_ERR_MI_FRAMINGERR" 	   Framing error,  0xeb
 *@li	\ref NDK_ERR_MI_COLLERR "NDK_ERR_MI_COLLERR" 	       Collison detected,    0xe8
 *@li	\ref NDK_ERR_MI_INTERFACEERR "NDK_ERR_MI_INTERFACEERR"    Reset interface error,  0xe6
 *@li	\ref NDK_ERR_MI_ACCESSTIMEOUT "NDK_ERR_MI_ACCESSTIMEOUT" 	 Receive timeout,  0xe5
 *@li	\ref NDK_ERR_MI_PROTOCOLERR "NDK_ERR_MI_PROTOCOLERR" 	      Protocol error,  0xe4
 *@li	\ref NDK_ERR_MI_QUIT "NDK_ERR_MI_QUIT" 	                    Abnomal quit,   0xe2
 *@li	\ref NDK_ERR_MI_PPSErr "NDK_ERR_MI_PPSErr" 	             PPS error,  0xe1
 *@li	\ref NDK_ERR_MI_SpiRequest "NDK_ERR_MI_SpiRequest" 	       Request SPI error,  0xa0
 *@li	\ref NDK_ERR_MI_NY_IMPLEMENTED "NDK_ERR_MI_NY_IMPLEMENTED" 	   Unknown error, 0x9c
 *@li	\ref NDK_ERR_MI_CardTypeErr "NDK_ERR_MI_CardTypeErr" 	     Wrong card type, 0x83
 *@li	\ref NDK_ERR_MI_ParaErrInIoctl "NDK_ERR_MI_ParaErrInIoctl" 	 Invalid parameter for ioctl, 0x82
 *@li	\ref NDK_ERR_MI_Para "NDK_ERR_MI_Para"                    Invalid internal parameter, 0xa9
*/
int NDK_RfidPiccActivate(uchar *psPiccType, int *pnDataLen,  uchar *psDataBuf);

/**
 *@brief	Activate Type A card
 *@param[in]   ucCid	For RATS command
 *@param[out]	pnDatalen	 Data length
 *@param[out]	psDatabuf	Data buffer(UID for Type A card, UID in psDataBuf[1]~[4] for Type B card and application or protocol information in other bytes)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				activation succeeded
 *@li	EM_NDK_ERR		activation failed
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
 *@li	\ref NDK_ERR_MI_NOTAGERR "NDK_ERR_MI_NOTAGERR" 	No card,		0xff
 *@li	\ref NDK_ERR_MI_CRCERR "NDK_ERR_MI_CRCERR" 	CRC error,        0xfe
 *@li	\ref NDK_ERR_MI_PARITYERR "NDK_ERR_MI_PARITYERR" 	Parity error,    oxfb
 *@li	\ref NDK_ERR_MI_BITCOUNTERR "NDK_ERR_MI_BITCOUNTERR" 	 Received bit error,  0xf5
 *@li	\ref NDK_ERR_MI_BYTECOUNTERR "NDK_ERR_MI_BYTECOUNTERR" 	  Received byte error, 0xf4
 *@li	\ref NDK_ERR_MI_WriteFifo "NDK_ERR_MI_WriteFifo" 	   FIFO write error, 0xf3
 *@li	\ref NDK_ERR_MI_ACCESSTIMEOUT "NDK_ERR_MI_ACCESSTIMEOUT" 	 Receive timeout,  0xe5
*/
int  NDK_RfidTypeARats(uchar ucCid, int *pnDatalen, uchar *psDatabuf);

/**
 *@brief	 Activate card (For EMV only)
 *@param[out] psPiccType	0xcc: TYPE-A card; 0xcb: TYPE-B card
 *@param[out]	pnDataLen	Data length
 *@param[out]	psDataBuf	Data buffer(UID for Type A card, UID in psDataBuf[1]~[4] for Type B card and application or protocol information in other bytes)
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
 *@li	\ref NDK_ERR_MI_NOTAGERR "NDK_ERR_MI_NOTAGERR" 	No card,		0xff
 *@li	\ref NDK_ERR_MI_CRCERR "NDK_ERR_MI_CRCERR" 	CRC error,        0xfe
 *@li	\ref NDK_ERR_MI_EMPTY "NDK_ERR_MI_EMPTY" 	Not empty,          0xfd
 *@li	\ref NDK_ERR_MI_AUTHERR "NDK_ERR_MI_AUTHERR" 	Authentication error,   0xfc
 *@li	\ref NDK_ERR_MI_PARITYERR "NDK_ERR_MI_PARITYERR" 	Parity error,    oxfb
 *@li	\ref NDK_ERR_MI_CODEERR "NDK_ERR_MI_CODEERR" 		Received code error, 0xfa
 *@li	\ref NDK_ERR_MI_SERNRERR "NDK_ERR_MI_SERNRERR" 	  Anti collision data check error, 0xf8
 *@li	\ref NDK_ERR_MI_KEYERR "NDK_ERR_MI_KEYERR" 	       Key error, 0xf7
 *@li	\ref NDK_ERR_MI_NOTAUTHERR "NDK_ERR_MI_NOTAUTHERR" 	 Not authenticated,  0xf6
 *@li	\ref NDK_ERR_MI_BITCOUNTERR "NDK_ERR_MI_BITCOUNTERR" 	 Received bit error,  0xf5
 *@li	\ref NDK_ERR_MI_BYTECOUNTERR "NDK_ERR_MI_BYTECOUNTERR" 	  Received byte error, 0xf4
 *@li	\ref NDK_ERR_MI_WriteFifo "NDK_ERR_MI_WriteFifo" 	   FIFO write error, 0xf3
 *@li	\ref NDK_ERR_MI_TRANSERR "NDK_ERR_MI_TRANSERR" 	       Transfer error, 0xf2
 *@li	\ref NDK_ERR_MI_WRITEERR "NDK_ERR_MI_WRITEERR" 	       Write error, 0xf1
 *@li	\ref NDK_ERR_MI_INCRERR "NDK_ERR_MI_INCRERR" 	       Increment error, 0xf0
 *@li	\ref NDK_ERR_MI_DECRERR "NDK_ERR_MI_DECRERR" 	        Decrement error,  0xef
 *@li	\ref NDK_ERR_MI_OVFLERR "NDK_ERR_MI_OVFLERR" 	       Overflow error, 0xed
 *@li	\ref NDK_ERR_MI_FRAMINGERR "NDK_ERR_MI_FRAMINGERR" 	   Framing error,  0xeb
 *@li	\ref NDK_ERR_MI_COLLERR "NDK_ERR_MI_COLLERR" 	       Collison detected,    0xe8
 *@li	\ref NDK_ERR_MI_INTERFACEERR "NDK_ERR_MI_INTERFACEERR"    Reset interface error,  0xe6
 *@li	\ref NDK_ERR_MI_ACCESSTIMEOUT "NDK_ERR_MI_ACCESSTIMEOUT" 	 Receive timeout,  0xe5
 *@li	\ref NDK_ERR_MI_PROTOCOLERR "NDK_ERR_MI_PROTOCOLERR" 	      Protocol error,  0xe4
 *@li	\ref NDK_ERR_MI_QUIT "NDK_ERR_MI_QUIT" 	                    Abnomal quit,   0xe2
 *@li	\ref NDK_ERR_MI_PPSErr "NDK_ERR_MI_PPSErr" 	             PPS error,  0xe1
 *@li	\ref NDK_ERR_MI_SpiRequest "NDK_ERR_MI_SpiRequest" 	       Request SPI error,  0xa0
 *@li	\ref NDK_ERR_MI_NY_IMPLEMENTED "NDK_ERR_MI_NY_IMPLEMENTED" 	   Unknown error, 0x9c
 *@li	\ref NDK_ERR_MI_CardTypeErr "NDK_ERR_MI_CardTypeErr" 	     Wrong card type, 0x83
 *@li	\ref NDK_ERR_MI_ParaErrInIoctl "NDK_ERR_MI_ParaErrInIoctl" 	 Invalid parameter for ioctl, 0x82
 *@li	\ref NDK_ERR_MI_Para "NDK_ERR_MI_Para"                    Invalid internal parameter, 0xa9
*/
int  NDK_RfidPiccActivate_EMV(uchar *psPiccType, int *pnDataLen,  uchar *psDataBuf);

/**
 *@brief	Deactivate card
 *@param[in] ucDelayMs	0: Always off; Non-zero: Time in milliseconds to power down then power up again.\n
 *                      (Powering down 6-10ms will invalidate card.)
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 	              Failed to call driver function
*/
int NDK_RfidPiccDeactivate(uchar ucDelayMs);

/**
 *@brief	Perform APDU
 *@param[in] nSendLen		Length of command sent
 *@param[in] psSendBuf		Command buffer
 *@param[out] 	pnRecvLen	Length of data received
 *@param[out]	psReceBuf	Data receiving buffer
 *@return
 *@li	\ref NDK_OK "NDK_OK"  		 Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"   	Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		  Failed to call driver function
*/
int NDK_RfidPiccApdu(int nSendLen, uchar *psSendBuf, int *pnRecvLen,  uchar *psReceBuf);

/**
 *@brief	Detect M1 card
 *@details Card type will be set to TYPE A
 *@param[in] ucReqCode		0: Request REQA; Non-zero: Wake up WUPA (Typically WUPA is needed)
 *@param[out] 	pnDataLen	 Length of data received (2 bytes)
 *@param[out]	psDataBuf		Data receiving buffer
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
 *@li	\ref NDK_ERR_MI_NOTAGERR "NDK_ERR_MI_NOTAGERR" 	No card,		0xff
 *@li	\ref NDK_ERR_MI_CRCERR "NDK_ERR_MI_CRCERR" 	CRC error,        0xfe
 *@li	\ref NDK_ERR_MI_EMPTY "NDK_ERR_MI_EMPTY" 	Not empty,          0xfd
 *@li	\ref NDK_ERR_MI_AUTHERR "NDK_ERR_MI_AUTHERR" 	Authentication error,   0xfc
 *@li	\ref NDK_ERR_MI_PARITYERR "NDK_ERR_MI_PARITYERR" 	Parity error,    oxfb
 *@li	\ref NDK_ERR_MI_CODEERR "NDK_ERR_MI_CODEERR" 		Received code error, 0xfa
 *@li	\ref NDK_ERR_MI_SERNRERR "NDK_ERR_MI_SERNRERR" 	  Anti collision data check error, 0xf8
 *@li	\ref NDK_ERR_MI_KEYERR "NDK_ERR_MI_KEYERR" 	       Key error, 0xf7
 *@li	\ref NDK_ERR_MI_NOTAUTHERR "NDK_ERR_MI_NOTAUTHERR" 	 Not authenticated,  0xf6
 *@li	\ref NDK_ERR_MI_BITCOUNTERR "NDK_ERR_MI_BITCOUNTERR" 	 Received bit error,  0xf5
 *@li	\ref NDK_ERR_MI_BYTECOUNTERR "NDK_ERR_MI_BYTECOUNTERR" 	  Received byte error, 0xf4
 *@li	\ref NDK_ERR_MI_WriteFifo "NDK_ERR_MI_WriteFifo" 	   FIFO write error, 0xf3
 *@li	\ref NDK_ERR_MI_TRANSERR "NDK_ERR_MI_TRANSERR" 	       Transfer error, 0xf2
 *@li	\ref NDK_ERR_MI_WRITEERR "NDK_ERR_MI_WRITEERR" 	       Write error, 0xf1
 *@li	\ref NDK_ERR_MI_INCRERR "NDK_ERR_MI_INCRERR" 	       Increment error, 0xf0
 *@li	\ref NDK_ERR_MI_DECRERR "NDK_ERR_MI_DECRERR" 	        Decrement error,  0xef
 *@li	\ref NDK_ERR_MI_OVFLERR "NDK_ERR_MI_OVFLERR" 	       Overflow error, 0xed
 *@li	\ref NDK_ERR_MI_FRAMINGERR "NDK_ERR_MI_FRAMINGERR" 	   Framing error,  0xeb
 *@li	\ref NDK_ERR_MI_COLLERR "NDK_ERR_MI_COLLERR" 	       Collison detected,    0xe8
 *@li	\ref NDK_ERR_MI_INTERFACEERR "NDK_ERR_MI_INTERFACEERR"    Reset interface error,  0xe6
 *@li	\ref NDK_ERR_MI_ACCESSTIMEOUT "NDK_ERR_MI_ACCESSTIMEOUT" 	 Receive timeout,  0xe5
 *@li	\ref NDK_ERR_MI_PROTOCOLERR "NDK_ERR_MI_PROTOCOLERR" 	      Protocol error,  0xe4
 *@li	\ref NDK_ERR_MI_QUIT "NDK_ERR_MI_QUIT" 	                    Abnomal quit,   0xe2
 *@li	\ref NDK_ERR_MI_PPSErr "NDK_ERR_MI_PPSErr" 	             PPS error,  0xe1
 *@li	\ref NDK_ERR_MI_SpiRequest "NDK_ERR_MI_SpiRequest" 	       Request SPI error,  0xa0
 *@li	\ref NDK_ERR_MI_NY_IMPLEMENTED "NDK_ERR_MI_NY_IMPLEMENTED" 	   Unknown error, 0x9c
 *@li	\ref NDK_ERR_MI_CardTypeErr "NDK_ERR_MI_CardTypeErr" 	     Wrong card type, 0x83
 *@li	\ref NDK_ERR_MI_ParaErrInIoctl "NDK_ERR_MI_ParaErrInIoctl" 	 Invalid parameter for ioctl, 0x82
 *@li	\ref NDK_ERR_MI_Para "NDK_ERR_MI_Para"                    Invalid internal parameter, 0xa9
*/
int NDK_M1Request(uchar ucReqCode, int *pnDataLen, uchar *psDataBuf);

/**
 *@brief	Perform M1 card anti-collision
 *@details Usually used when card is detected
 *@param[out] pnDataLen	Length of date received(UID length)
 *@param[out]	psDataBuf	Data receiving buffer(UID)
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
 *@li	\ref NDK_ERR_MI_NOTAGERR "NDK_ERR_MI_NOTAGERR" 	No card,		0xff
 *@li	\ref NDK_ERR_MI_CRCERR "NDK_ERR_MI_CRCERR" 	CRC error,        0xfe
 *@li	\ref NDK_ERR_MI_EMPTY "NDK_ERR_MI_EMPTY" 	Not empty,          0xfd
 *@li	\ref NDK_ERR_MI_AUTHERR "NDK_ERR_MI_AUTHERR" 	Authentication error,   0xfc
 *@li	\ref NDK_ERR_MI_PARITYERR "NDK_ERR_MI_PARITYERR" 	Parity error,    oxfb
 *@li	\ref NDK_ERR_MI_CODEERR "NDK_ERR_MI_CODEERR" 		Received code error, 0xfa
 *@li	\ref NDK_ERR_MI_SERNRERR "NDK_ERR_MI_SERNRERR" 	  Anti collision data check error, 0xf8
 *@li	\ref NDK_ERR_MI_KEYERR "NDK_ERR_MI_KEYERR" 	       Key error, 0xf7
 *@li	\ref NDK_ERR_MI_NOTAUTHERR "NDK_ERR_MI_NOTAUTHERR" 	 Not authenticated,  0xf6
 *@li	\ref NDK_ERR_MI_BITCOUNTERR "NDK_ERR_MI_BITCOUNTERR" 	 Received bit error,  0xf5
 *@li	\ref NDK_ERR_MI_BYTECOUNTERR "NDK_ERR_MI_BYTECOUNTERR" 	  Received byte error, 0xf4
 *@li	\ref NDK_ERR_MI_WriteFifo "NDK_ERR_MI_WriteFifo" 	   FIFO write error, 0xf3
 *@li	\ref NDK_ERR_MI_TRANSERR "NDK_ERR_MI_TRANSERR" 	       Transfer error, 0xf2
 *@li	\ref NDK_ERR_MI_WRITEERR "NDK_ERR_MI_WRITEERR" 	       Write error, 0xf1
 *@li	\ref NDK_ERR_MI_INCRERR "NDK_ERR_MI_INCRERR" 	       Increment error, 0xf0
 *@li	\ref NDK_ERR_MI_DECRERR "NDK_ERR_MI_DECRERR" 	        Decrement error,  0xef
 *@li	\ref NDK_ERR_MI_OVFLERR "NDK_ERR_MI_OVFLERR" 	       Overflow error, 0xed
 *@li	\ref NDK_ERR_MI_FRAMINGERR "NDK_ERR_MI_FRAMINGERR" 	   Framing error,  0xeb
 *@li	\ref NDK_ERR_MI_COLLERR "NDK_ERR_MI_COLLERR" 	       Collison detected,    0xe8
 *@li	\ref NDK_ERR_MI_INTERFACEERR "NDK_ERR_MI_INTERFACEERR"    Reset interface error,  0xe6
 *@li	\ref NDK_ERR_MI_ACCESSTIMEOUT "NDK_ERR_MI_ACCESSTIMEOUT" 	 Receive timeout,  0xe5
 *@li	\ref NDK_ERR_MI_PROTOCOLERR "NDK_ERR_MI_PROTOCOLERR" 	      Protocol error,  0xe4
 *@li	\ref NDK_ERR_MI_QUIT "NDK_ERR_MI_QUIT" 	                    Abnomal quit,   0xe2
 *@li	\ref NDK_ERR_MI_PPSErr "NDK_ERR_MI_PPSErr" 	             PPS error,  0xe1
 *@li	\ref NDK_ERR_MI_SpiRequest "NDK_ERR_MI_SpiRequest" 	       Request SPI error,  0xa0
 *@li	\ref NDK_ERR_MI_NY_IMPLEMENTED "NDK_ERR_MI_NY_IMPLEMENTED" 	   Unknown error, 0x9c
 *@li	\ref NDK_ERR_MI_CardTypeErr "NDK_ERR_MI_CardTypeErr" 	     Wrong card type, 0x83
 *@li	\ref NDK_ERR_MI_ParaErrInIoctl "NDK_ERR_MI_ParaErrInIoctl" 	 Invalid parameter for ioctl, 0x82
 *@li	\ref NDK_ERR_MI_Para "NDK_ERR_MI_Para"                    Invalid internal parameter, 0xa9

*/
int NDK_M1Anti(int *pnDataLen, uchar *psDataBuf);

/**
 *@brief   Perfoem M1 card anti-collision for cascaded UID
 *@param[in] ucSelCode	PICC_ANTICOLL1/PICC_ANTICOLL2/PICC_ANTICOLL3
 *@param[out] 	pnDataLen	 Received Data length(UID length)
 *@param[out]	psDataBuf	  Received Data buffer(UID)
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 			Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		     Failed to call driver function
*/
int  NDK_M1Anti_SEL(uchar ucSelCode, int *pnDataLen, uchar *psDataBuf);

/**
 *@brief		Select M1 card
 *@details Usually used when \ref NDK_M1Anti "NDK_M1Anti" succeeds
 *@param[in] 	nUidLen			UID length
 *@param[in] 	psUidBuf          UID data buffer
 *@param[out] 	psSakBuf	    Data buffer for card selection (1 byte SAK)
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
 *@li	\ref NDK_ERR_MI_NOTAGERR "NDK_ERR_MI_NOTAGERR" 	No card,		0xff
 *@li	\ref NDK_ERR_MI_CRCERR "NDK_ERR_MI_CRCERR" 	CRC error,        0xfe
 *@li	\ref NDK_ERR_MI_EMPTY "NDK_ERR_MI_EMPTY" 	Not empty,          0xfd
 *@li	\ref NDK_ERR_MI_AUTHERR "NDK_ERR_MI_AUTHERR" 	Authentication error,   0xfc
 *@li	\ref NDK_ERR_MI_PARITYERR "NDK_ERR_MI_PARITYERR" 	Parity error,    oxfb
 *@li	\ref NDK_ERR_MI_CODEERR "NDK_ERR_MI_CODEERR" 		Received code error, 0xfa
 *@li	\ref NDK_ERR_MI_SERNRERR "NDK_ERR_MI_SERNRERR" 	  Anti collision data check error, 0xf8
 *@li	\ref NDK_ERR_MI_KEYERR "NDK_ERR_MI_KEYERR" 	       Key error, 0xf7
 *@li	\ref NDK_ERR_MI_NOTAUTHERR "NDK_ERR_MI_NOTAUTHERR" 	 Not authenticated,  0xf6
 *@li	\ref NDK_ERR_MI_BITCOUNTERR "NDK_ERR_MI_BITCOUNTERR" 	 Received bit error,  0xf5
 *@li	\ref NDK_ERR_MI_BYTECOUNTERR "NDK_ERR_MI_BYTECOUNTERR" 	  Received byte error, 0xf4
 *@li	\ref NDK_ERR_MI_WriteFifo "NDK_ERR_MI_WriteFifo" 	   FIFO write error, 0xf3
 *@li	\ref NDK_ERR_MI_TRANSERR "NDK_ERR_MI_TRANSERR" 	       Transfer error, 0xf2
 *@li	\ref NDK_ERR_MI_WRITEERR "NDK_ERR_MI_WRITEERR" 	       Write error, 0xf1
 *@li	\ref NDK_ERR_MI_INCRERR "NDK_ERR_MI_INCRERR" 	       Increment error, 0xf0
 *@li	\ref NDK_ERR_MI_DECRERR "NDK_ERR_MI_DECRERR" 	        Decrement error,  0xef
 *@li	\ref NDK_ERR_MI_OVFLERR "NDK_ERR_MI_OVFLERR" 	       Overflow error, 0xed
 *@li	\ref NDK_ERR_MI_FRAMINGERR "NDK_ERR_MI_FRAMINGERR" 	   Framing error,  0xeb
 *@li	\ref NDK_ERR_MI_COLLERR "NDK_ERR_MI_COLLERR" 	       Collison detected,    0xe8
 *@li	\ref NDK_ERR_MI_INTERFACEERR "NDK_ERR_MI_INTERFACEERR"    Reset interface error,  0xe6
 *@li	\ref NDK_ERR_MI_ACCESSTIMEOUT "NDK_ERR_MI_ACCESSTIMEOUT" 	 Receive timeout,  0xe5
 *@li	\ref NDK_ERR_MI_PROTOCOLERR "NDK_ERR_MI_PROTOCOLERR" 	      Protocol error,  0xe4
 *@li	\ref NDK_ERR_MI_QUIT "NDK_ERR_MI_QUIT" 	                    Abnomal quit,   0xe2
 *@li	\ref NDK_ERR_MI_PPSErr "NDK_ERR_MI_PPSErr" 	             PPS error,  0xe1
 *@li	\ref NDK_ERR_MI_SpiRequest "NDK_ERR_MI_SpiRequest" 	       Request SPI error,  0xa0
 *@li	\ref NDK_ERR_MI_NY_IMPLEMENTED "NDK_ERR_MI_NY_IMPLEMENTED" 	   Unknown error, 0x9c
 *@li	\ref NDK_ERR_MI_CardTypeErr "NDK_ERR_MI_CardTypeErr" 	     Wrong card type, 0x83
 *@li	\ref NDK_ERR_MI_ParaErrInIoctl "NDK_ERR_MI_ParaErrInIoctl" 	 Invalid parameter for ioctl, 0x82
 *@li	\ref NDK_ERR_MI_Para "NDK_ERR_MI_Para"                    Invalid internal parameter, 0xa9
*/
int NDK_M1Select(int nUidLen, uchar *psUidBuf, uchar *psSakBuf);

/**
 *@brief		Select M1 card for cascaded UID
 *@param[in]  ucSelCode		0: Request REQA; Non-zero: Wake up WUPA (Typically WUPA is needed)
 *@param[in] 	nUidLen			UID length
 *@param[in] 	psUidBuf          UID data buffer
 *@param[out] 	psSakBuf	    Data buffer for card selection (1 byte SAK)
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
 *@li	\ref NDK_ERR_MI_NOTAGERR "NDK_ERR_MI_NOTAGERR" 	No card,		0xff
 *@li	\ref NDK_ERR_MI_CRCERR "NDK_ERR_MI_CRCERR" 	CRC error,        0xfe
 *@li	\ref NDK_ERR_MI_EMPTY "NDK_ERR_MI_EMPTY" 	Not empty,          0xfd
 *@li	\ref NDK_ERR_MI_AUTHERR "NDK_ERR_MI_AUTHERR" 	Authentication error,   0xfc
 *@li	\ref NDK_ERR_MI_PARITYERR "NDK_ERR_MI_PARITYERR" 	Parity error,    oxfb
 *@li	\ref NDK_ERR_MI_CODEERR "NDK_ERR_MI_CODEERR" 		Received code error, 0xfa
 *@li	\ref NDK_ERR_MI_SERNRERR "NDK_ERR_MI_SERNRERR" 	  Anti collision data check error, 0xf8
 *@li	\ref NDK_ERR_MI_KEYERR "NDK_ERR_MI_KEYERR" 	       Key error, 0xf7
 *@li	\ref NDK_ERR_MI_NOTAUTHERR "NDK_ERR_MI_NOTAUTHERR" 	 Not authenticated,  0xf6
 *@li	\ref NDK_ERR_MI_BITCOUNTERR "NDK_ERR_MI_BITCOUNTERR" 	 Received bit error,  0xf5
 *@li	\ref NDK_ERR_MI_BYTECOUNTERR "NDK_ERR_MI_BYTECOUNTERR" 	  Received byte error, 0xf4
 *@li	\ref NDK_ERR_MI_WriteFifo "NDK_ERR_MI_WriteFifo" 	   FIFO write error, 0xf3
 *@li	\ref NDK_ERR_MI_TRANSERR "NDK_ERR_MI_TRANSERR" 	       Transfer error, 0xf2
 *@li	\ref NDK_ERR_MI_WRITEERR "NDK_ERR_MI_WRITEERR" 	       Write error, 0xf1
 *@li	\ref NDK_ERR_MI_INCRERR "NDK_ERR_MI_INCRERR" 	       Increment error, 0xf0
 *@li	\ref NDK_ERR_MI_DECRERR "NDK_ERR_MI_DECRERR" 	        Decrement error,  0xef
 *@li	\ref NDK_ERR_MI_OVFLERR "NDK_ERR_MI_OVFLERR" 	       Overflow error, 0xed
 *@li	\ref NDK_ERR_MI_FRAMINGERR "NDK_ERR_MI_FRAMINGERR" 	   Framing error,  0xeb
 *@li	\ref NDK_ERR_MI_COLLERR "NDK_ERR_MI_COLLERR" 	       Collison detected,    0xe8
 *@li	\ref NDK_ERR_MI_INTERFACEERR "NDK_ERR_MI_INTERFACEERR"    Reset interface error,  0xe6
 *@li	\ref NDK_ERR_MI_ACCESSTIMEOUT "NDK_ERR_MI_ACCESSTIMEOUT" 	 Receive timeout,  0xe5
 *@li	\ref NDK_ERR_MI_PROTOCOLERR "NDK_ERR_MI_PROTOCOLERR" 	      Protocol error,  0xe4
 *@li	\ref NDK_ERR_MI_QUIT "NDK_ERR_MI_QUIT" 	                    Abnomal quit,   0xe2
 *@li	\ref NDK_ERR_MI_PPSErr "NDK_ERR_MI_PPSErr" 	             PPS error,  0xe1
 *@li	\ref NDK_ERR_MI_SpiRequest "NDK_ERR_MI_SpiRequest" 	       Request SPI error,  0xa0
 *@li	\ref NDK_ERR_MI_NY_IMPLEMENTED "NDK_ERR_MI_NY_IMPLEMENTED" 	   Unknown error, 0x9c
 *@li	\ref NDK_ERR_MI_CardTypeErr "NDK_ERR_MI_CardTypeErr" 	     Wrong card type, 0x83
 *@li	\ref NDK_ERR_MI_ParaErrInIoctl "NDK_ERR_MI_ParaErrInIoctl" 	 Invalid parameter for ioctl, 0x82
 *@li	\ref NDK_ERR_MI_Para "NDK_ERR_MI_Para"                    Invalid internal parameter, 0xa9
*/
int  NDK_M1Select_SEL(uchar ucSelCode, int nUidLen, uchar *psUidBuf, uchar *psSakBuf);

/**
 *@brief	Store authentication key for M1 card
 *@details Same key will be stroed only once
 *@param[in] ucKeyType		Authentication key type. (0x00: Type A; 0x01: Type B)
 *@param[in]   ucKeyNum	Key index:0-15 (Type A and B has 16 keys for each)
 *@param[in] psKeyData		 Key data,6 bytes
 *@return
 *@li	\ref NDK_OK "NDK_OK"  		 	Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 	 Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 		              	Fail
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	 	Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		      Failed to call driver function
*/
int NDK_M1KeyStore(uchar ucKeyType,  uchar ucKeyNum, uchar *psKeyData);

/**
 *@brief	Load authentication key for M1 card
 *@details Same key will be loaded only once
 *@param[in] ucKeyType		Authentication key type. (0x00: Type A; 0x01: Type B)
 *@param[in]   ucKeyNum	Key index:0-15 (Type A and B has 16 keys for each)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  		 	Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR" 		              Fail
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		     Failed to call driver function
*/
int NDK_M1KeyLoad(uchar ucKeyType,  uchar ucKeyNum);

/**
 *@brief	Authenticate M1 card with external key
 *@param[in] nUidLen	UID length
 *@param[in] psUidBuf	UID data
 *@param[in] ucKeyType		Authentication key type. (0x00: Type A; 0x01: Type B)
 *@param[in] psKeyData	Key data(6 bytes)
 *@param[in]   ucBlockNum	Block to be authenticated
 *@return
 *@li	\ref NDK_OK "NDK_OK"  			  	Success
 *@li	\ref NDK_ERR "NDK_ERR" 		            	Fail
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		   Failed to call driver function
*/
int NDK_M1ExternalAuthen(int nUidLen, uchar *psUidBuf, uchar ucKeyType, uchar *psKeyData, uchar ucBlockNum);

/**
 *@brief		Read M1 card block
 *@param[in] ucBlockNum	Block to read
 *@param[out]	pnDataLen	Length of block data read
 *@param[out]	psBlockData	Block data
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
 *@li	\ref NDK_ERR_MI_NOTAGERR "NDK_ERR_MI_NOTAGERR" 	No card,		0xff
 *@li	\ref NDK_ERR_MI_CRCERR "NDK_ERR_MI_CRCERR" 	CRC error,        0xfe
 *@li	\ref NDK_ERR_MI_EMPTY "NDK_ERR_MI_EMPTY" 	Not empty,          0xfd
 *@li	\ref NDK_ERR_MI_AUTHERR "NDK_ERR_MI_AUTHERR" 	Authentication error,   0xfc
 *@li	\ref NDK_ERR_MI_PARITYERR "NDK_ERR_MI_PARITYERR" 	Parity error,    oxfb
 *@li	\ref NDK_ERR_MI_CODEERR "NDK_ERR_MI_CODEERR" 		Received code error, 0xfa
 *@li	\ref NDK_ERR_MI_SERNRERR "NDK_ERR_MI_SERNRERR" 	  Anti collision data check error, 0xf8
 *@li	\ref NDK_ERR_MI_KEYERR "NDK_ERR_MI_KEYERR" 	       Key error, 0xf7
 *@li	\ref NDK_ERR_MI_NOTAUTHERR "NDK_ERR_MI_NOTAUTHERR" 	 Not authenticated,  0xf6
 *@li	\ref NDK_ERR_MI_BITCOUNTERR "NDK_ERR_MI_BITCOUNTERR" 	 Received bit error,  0xf5
 *@li	\ref NDK_ERR_MI_BYTECOUNTERR "NDK_ERR_MI_BYTECOUNTERR" 	  Received byte error, 0xf4
 *@li	\ref NDK_ERR_MI_WriteFifo "NDK_ERR_MI_WriteFifo" 	   FIFO write error, 0xf3
 *@li	\ref NDK_ERR_MI_TRANSERR "NDK_ERR_MI_TRANSERR" 	       Transfer error, 0xf2
 *@li	\ref NDK_ERR_MI_WRITEERR "NDK_ERR_MI_WRITEERR" 	       Write error, 0xf1
 *@li	\ref NDK_ERR_MI_INCRERR "NDK_ERR_MI_INCRERR" 	       Increment error, 0xf0
 *@li	\ref NDK_ERR_MI_DECRERR "NDK_ERR_MI_DECRERR" 	        Decrement error,  0xef
 *@li	\ref NDK_ERR_MI_OVFLERR "NDK_ERR_MI_OVFLERR" 	       Overflow error, 0xed
 *@li	\ref NDK_ERR_MI_FRAMINGERR "NDK_ERR_MI_FRAMINGERR" 	   Framing error,  0xeb
 *@li	\ref NDK_ERR_MI_COLLERR "NDK_ERR_MI_COLLERR" 	       Collison detected,    0xe8
 *@li	\ref NDK_ERR_MI_INTERFACEERR "NDK_ERR_MI_INTERFACEERR"    Reset interface error,  0xe6
 *@li	\ref NDK_ERR_MI_ACCESSTIMEOUT "NDK_ERR_MI_ACCESSTIMEOUT" 	 Receive timeout,  0xe5
 *@li	\ref NDK_ERR_MI_PROTOCOLERR "NDK_ERR_MI_PROTOCOLERR" 	      Protocol error,  0xe4
 *@li	\ref NDK_ERR_MI_QUIT "NDK_ERR_MI_QUIT" 	                    Abnomal quit,   0xe2
 *@li	\ref NDK_ERR_MI_PPSErr "NDK_ERR_MI_PPSErr" 	             PPS error,  0xe1
 *@li	\ref NDK_ERR_MI_SpiRequest "NDK_ERR_MI_SpiRequest" 	       Request SPI error,  0xa0
 *@li	\ref NDK_ERR_MI_NY_IMPLEMENTED "NDK_ERR_MI_NY_IMPLEMENTED" 	   Unknown error, 0x9c
 *@li	\ref NDK_ERR_MI_CardTypeErr "NDK_ERR_MI_CardTypeErr" 	     Wrong card type, 0x83
 *@li	\ref NDK_ERR_MI_ParaErrInIoctl "NDK_ERR_MI_ParaErrInIoctl" 	 Invalid parameter for ioctl, 0x82
 *@li	\ref NDK_ERR_MI_Para "NDK_ERR_MI_Para"                    Invalid internal parameter, 0xa9
*/
int NDK_M1Read(uchar ucBlockNum, int *pnDataLen, uchar *psBlockData);

/**
 *@brief		Write M1 card block
 *@param[in] ucBlockNum	Block to write
 *@param[out]	pnDataLen	Length of block data write
 *@param[out]	psBlockData	Block data
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
 *@li	\ref NDK_ERR_MI_NOTAGERR "NDK_ERR_MI_NOTAGERR" 	No card,		0xff
 *@li	\ref NDK_ERR_MI_CRCERR "NDK_ERR_MI_CRCERR" 	CRC error,        0xfe
 *@li	\ref NDK_ERR_MI_EMPTY "NDK_ERR_MI_EMPTY" 	Not empty,          0xfd
 *@li	\ref NDK_ERR_MI_AUTHERR "NDK_ERR_MI_AUTHERR" 	Authentication error,   0xfc
 *@li	\ref NDK_ERR_MI_PARITYERR "NDK_ERR_MI_PARITYERR" 	Parity error,    oxfb
 *@li	\ref NDK_ERR_MI_CODEERR "NDK_ERR_MI_CODEERR" 		Received code error, 0xfa
 *@li	\ref NDK_ERR_MI_SERNRERR "NDK_ERR_MI_SERNRERR" 	  Anti collision data check error, 0xf8
 *@li	\ref NDK_ERR_MI_KEYERR "NDK_ERR_MI_KEYERR" 	       Key error, 0xf7
 *@li	\ref NDK_ERR_MI_NOTAUTHERR "NDK_ERR_MI_NOTAUTHERR" 	 Not authenticated,  0xf6
 *@li	\ref NDK_ERR_MI_BITCOUNTERR "NDK_ERR_MI_BITCOUNTERR" 	 Received bit error,  0xf5
 *@li	\ref NDK_ERR_MI_BYTECOUNTERR "NDK_ERR_MI_BYTECOUNTERR" 	  Received byte error, 0xf4
 *@li	\ref NDK_ERR_MI_WriteFifo "NDK_ERR_MI_WriteFifo" 	   FIFO write error, 0xf3
 *@li	\ref NDK_ERR_MI_TRANSERR "NDK_ERR_MI_TRANSERR" 	       Transfer error, 0xf2
 *@li	\ref NDK_ERR_MI_WRITEERR "NDK_ERR_MI_WRITEERR" 	       Write error, 0xf1
 *@li	\ref NDK_ERR_MI_INCRERR "NDK_ERR_MI_INCRERR" 	       Increment error, 0xf0
 *@li	\ref NDK_ERR_MI_DECRERR "NDK_ERR_MI_DECRERR" 	        Decrement error,  0xef
 *@li	\ref NDK_ERR_MI_OVFLERR "NDK_ERR_MI_OVFLERR" 	       Overflow error, 0xed
 *@li	\ref NDK_ERR_MI_FRAMINGERR "NDK_ERR_MI_FRAMINGERR" 	   Framing error,  0xeb
 *@li	\ref NDK_ERR_MI_COLLERR "NDK_ERR_MI_COLLERR" 	       Collison detected,    0xe8
 *@li	\ref NDK_ERR_MI_INTERFACEERR "NDK_ERR_MI_INTERFACEERR"    Reset interface error,  0xe6
 *@li	\ref NDK_ERR_MI_ACCESSTIMEOUT "NDK_ERR_MI_ACCESSTIMEOUT" 	 Receive timeout,  0xe5
 *@li	\ref NDK_ERR_MI_PROTOCOLERR "NDK_ERR_MI_PROTOCOLERR" 	      Protocol error,  0xe4
 *@li	\ref NDK_ERR_MI_QUIT "NDK_ERR_MI_QUIT" 	                    Abnomal quit,   0xe2
 *@li	\ref NDK_ERR_MI_PPSErr "NDK_ERR_MI_PPSErr" 	             PPS error,  0xe1
 *@li	\ref NDK_ERR_MI_SpiRequest "NDK_ERR_MI_SpiRequest" 	       Request SPI error,  0xa0
 *@li	\ref NDK_ERR_MI_NY_IMPLEMENTED "NDK_ERR_MI_NY_IMPLEMENTED" 	   Unknown error, 0x9c
 *@li	\ref NDK_ERR_MI_CardTypeErr "NDK_ERR_MI_CardTypeErr" 	     Wrong card type, 0x83
 *@li	\ref NDK_ERR_MI_ParaErrInIoctl "NDK_ERR_MI_ParaErrInIoctl" 	 Invalid parameter for ioctl, 0x82
 *@li	\ref NDK_ERR_MI_Para "NDK_ERR_MI_Para"                    Invalid internal parameter, 0xa9
*/
int NDK_M1Write(uchar ucBlockNum, int *pnDataLen, uchar *psBlockData);

/**
 *@brief	 Increment M1 card block
 *@param[in] ucBlockNum	Block to increment.
 *@param[in] nDataLen	Incremental data length (4 bytes)
 *@param[in] psDataBuf	Incremental data
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
 *@li	\ref NDK_ERR_MI_NOTAGERR "NDK_ERR_MI_NOTAGERR" 	No card,		0xff
 *@li	\ref NDK_ERR_MI_CRCERR "NDK_ERR_MI_CRCERR" 	CRC error,        0xfe
 *@li	\ref NDK_ERR_MI_EMPTY "NDK_ERR_MI_EMPTY" 	Not empty,          0xfd
 *@li	\ref NDK_ERR_MI_AUTHERR "NDK_ERR_MI_AUTHERR" 	Authentication error,   0xfc
 *@li	\ref NDK_ERR_MI_PARITYERR "NDK_ERR_MI_PARITYERR" 	Parity error,    oxfb
 *@li	\ref NDK_ERR_MI_CODEERR "NDK_ERR_MI_CODEERR" 		Received code error, 0xfa
 *@li	\ref NDK_ERR_MI_SERNRERR "NDK_ERR_MI_SERNRERR" 	  Anti collision data check error, 0xf8
 *@li	\ref NDK_ERR_MI_KEYERR "NDK_ERR_MI_KEYERR" 	       Key error, 0xf7
 *@li	\ref NDK_ERR_MI_NOTAUTHERR "NDK_ERR_MI_NOTAUTHERR" 	 Not authenticated,  0xf6
 *@li	\ref NDK_ERR_MI_BITCOUNTERR "NDK_ERR_MI_BITCOUNTERR" 	 Received bit error,  0xf5
 *@li	\ref NDK_ERR_MI_BYTECOUNTERR "NDK_ERR_MI_BYTECOUNTERR" 	  Received byte error, 0xf4
 *@li	\ref NDK_ERR_MI_WriteFifo "NDK_ERR_MI_WriteFifo" 	   FIFO write error, 0xf3
 *@li	\ref NDK_ERR_MI_TRANSERR "NDK_ERR_MI_TRANSERR" 	       Transfer error, 0xf2
 *@li	\ref NDK_ERR_MI_WRITEERR "NDK_ERR_MI_WRITEERR" 	       Write error, 0xf1
 *@li	\ref NDK_ERR_MI_INCRERR "NDK_ERR_MI_INCRERR" 	       Increment error, 0xf0
 *@li	\ref NDK_ERR_MI_DECRERR "NDK_ERR_MI_DECRERR" 	        Decrement error,  0xef
 *@li	\ref NDK_ERR_MI_OVFLERR "NDK_ERR_MI_OVFLERR" 	       Overflow error, 0xed
 *@li	\ref NDK_ERR_MI_FRAMINGERR "NDK_ERR_MI_FRAMINGERR" 	   Framing error,  0xeb
 *@li	\ref NDK_ERR_MI_COLLERR "NDK_ERR_MI_COLLERR" 	       Collison detected,    0xe8
 *@li	\ref NDK_ERR_MI_INTERFACEERR "NDK_ERR_MI_INTERFACEERR"    Reset interface error,  0xe6
 *@li	\ref NDK_ERR_MI_ACCESSTIMEOUT "NDK_ERR_MI_ACCESSTIMEOUT" 	 Receive timeout,  0xe5
 *@li	\ref NDK_ERR_MI_PROTOCOLERR "NDK_ERR_MI_PROTOCOLERR" 	      Protocol error,  0xe4
 *@li	\ref NDK_ERR_MI_QUIT "NDK_ERR_MI_QUIT" 	                    Abnomal quit,   0xe2
 *@li	\ref NDK_ERR_MI_PPSErr "NDK_ERR_MI_PPSErr" 	             PPS error,  0xe1
 *@li	\ref NDK_ERR_MI_SpiRequest "NDK_ERR_MI_SpiRequest" 	       Request SPI error,  0xa0
 *@li	\ref NDK_ERR_MI_NY_IMPLEMENTED "NDK_ERR_MI_NY_IMPLEMENTED" 	   Unknown error, 0x9c
 *@li	\ref NDK_ERR_MI_CardTypeErr "NDK_ERR_MI_CardTypeErr" 	     Wrong card type, 0x83
 *@li	\ref NDK_ERR_MI_ParaErrInIoctl "NDK_ERR_MI_ParaErrInIoctl" 	 Invalid parameter for ioctl, 0x82
 *@li	\ref NDK_ERR_MI_Para "NDK_ERR_MI_Para"                    Invalid internal parameter, 0xa9
*/
int NDK_M1Increment(uchar ucBlockNum, int nDataLen, uchar *psDataBuf);

/**
 *@brief	 Decrement M1 card block
 *@param[in] ucBlockNum	Block to decrement.
 *@param[in] nDataLen	Decremental data length (4 bytes)
 *@param[in] psDataBuf	Decremental data
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
 *@li	\ref NDK_ERR_MI_NOTAGERR "NDK_ERR_MI_NOTAGERR" 	No card,		0xff
 *@li	\ref NDK_ERR_MI_CRCERR "NDK_ERR_MI_CRCERR" 	CRC error,        0xfe
 *@li	\ref NDK_ERR_MI_EMPTY "NDK_ERR_MI_EMPTY" 	Not empty,          0xfd
 *@li	\ref NDK_ERR_MI_AUTHERR "NDK_ERR_MI_AUTHERR" 	Authentication error,   0xfc
 *@li	\ref NDK_ERR_MI_PARITYERR "NDK_ERR_MI_PARITYERR" 	Parity error,    oxfb
 *@li	\ref NDK_ERR_MI_CODEERR "NDK_ERR_MI_CODEERR" 		Received code error, 0xfa
 *@li	\ref NDK_ERR_MI_SERNRERR "NDK_ERR_MI_SERNRERR" 	  Anti collision data check error, 0xf8
 *@li	\ref NDK_ERR_MI_KEYERR "NDK_ERR_MI_KEYERR" 	       Key error, 0xf7
 *@li	\ref NDK_ERR_MI_NOTAUTHERR "NDK_ERR_MI_NOTAUTHERR" 	 Not authenticated,  0xf6
 *@li	\ref NDK_ERR_MI_BITCOUNTERR "NDK_ERR_MI_BITCOUNTERR" 	 Received bit error,  0xf5
 *@li	\ref NDK_ERR_MI_BYTECOUNTERR "NDK_ERR_MI_BYTECOUNTERR" 	  Received byte error, 0xf4
 *@li	\ref NDK_ERR_MI_WriteFifo "NDK_ERR_MI_WriteFifo" 	   FIFO write error, 0xf3
 *@li	\ref NDK_ERR_MI_TRANSERR "NDK_ERR_MI_TRANSERR" 	       Transfer error, 0xf2
 *@li	\ref NDK_ERR_MI_WRITEERR "NDK_ERR_MI_WRITEERR" 	       Write error, 0xf1
 *@li	\ref NDK_ERR_MI_INCRERR "NDK_ERR_MI_INCRERR" 	       Increment error, 0xf0
 *@li	\ref NDK_ERR_MI_DECRERR "NDK_ERR_MI_DECRERR" 	        Decrement error,  0xef
 *@li	\ref NDK_ERR_MI_OVFLERR "NDK_ERR_MI_OVFLERR" 	       Overflow error, 0xed
 *@li	\ref NDK_ERR_MI_FRAMINGERR "NDK_ERR_MI_FRAMINGERR" 	   Framing error,  0xeb
 *@li	\ref NDK_ERR_MI_COLLERR "NDK_ERR_MI_COLLERR" 	       Collison detected,    0xe8
 *@li	\ref NDK_ERR_MI_INTERFACEERR "NDK_ERR_MI_INTERFACEERR"    Reset interface error,  0xe6
 *@li	\ref NDK_ERR_MI_ACCESSTIMEOUT "NDK_ERR_MI_ACCESSTIMEOUT" 	 Receive timeout,  0xe5
 *@li	\ref NDK_ERR_MI_PROTOCOLERR "NDK_ERR_MI_PROTOCOLERR" 	      Protocol error,  0xe4
 *@li	\ref NDK_ERR_MI_QUIT "NDK_ERR_MI_QUIT" 	                    Abnomal quit,   0xe2
 *@li	\ref NDK_ERR_MI_PPSErr "NDK_ERR_MI_PPSErr" 	             PPS error,  0xe1
 *@li	\ref NDK_ERR_MI_SpiRequest "NDK_ERR_MI_SpiRequest" 	       Request SPI error,  0xa0
 *@li	\ref NDK_ERR_MI_NY_IMPLEMENTED "NDK_ERR_MI_NY_IMPLEMENTED" 	   Unknown error, 0x9c
 *@li	\ref NDK_ERR_MI_CardTypeErr "NDK_ERR_MI_CardTypeErr" 	     Wrong card type, 0x83
 *@li	\ref NDK_ERR_MI_ParaErrInIoctl "NDK_ERR_MI_ParaErrInIoctl" 	 Invalid parameter for ioctl, 0x82
 *@li	\ref NDK_ERR_MI_Para "NDK_ERR_MI_Para"                    Invalid internal parameter, 0xa9
*/
int NDK_M1Decrement(uchar ucBlockNum, int nDataLen, uchar *psDataBuf);

/**
 *@brief	Transfer M1 card block after increment/decrement
 *@param[in] ucBlockNum	Block to transfer
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
 *@li	\ref NDK_ERR_MI_NOTAGERR "NDK_ERR_MI_NOTAGERR" 	No card,		0xff
 *@li	\ref NDK_ERR_MI_CRCERR "NDK_ERR_MI_CRCERR" 	CRC error,        0xfe
 *@li	\ref NDK_ERR_MI_EMPTY "NDK_ERR_MI_EMPTY" 	Not empty,          0xfd
 *@li	\ref NDK_ERR_MI_AUTHERR "NDK_ERR_MI_AUTHERR" 	Authentication error,   0xfc
 *@li	\ref NDK_ERR_MI_PARITYERR "NDK_ERR_MI_PARITYERR" 	Parity error,    oxfb
 *@li	\ref NDK_ERR_MI_CODEERR "NDK_ERR_MI_CODEERR" 		Received code error, 0xfa
 *@li	\ref NDK_ERR_MI_SERNRERR "NDK_ERR_MI_SERNRERR" 	  Anti collision data check error, 0xf8
 *@li	\ref NDK_ERR_MI_KEYERR "NDK_ERR_MI_KEYERR" 	       Key error, 0xf7
 *@li	\ref NDK_ERR_MI_NOTAUTHERR "NDK_ERR_MI_NOTAUTHERR" 	 Not authenticated,  0xf6
 *@li	\ref NDK_ERR_MI_BITCOUNTERR "NDK_ERR_MI_BITCOUNTERR" 	 Received bit error,  0xf5
 *@li	\ref NDK_ERR_MI_BYTECOUNTERR "NDK_ERR_MI_BYTECOUNTERR" 	  Received byte error, 0xf4
 *@li	\ref NDK_ERR_MI_WriteFifo "NDK_ERR_MI_WriteFifo" 	   FIFO write error, 0xf3
 *@li	\ref NDK_ERR_MI_TRANSERR "NDK_ERR_MI_TRANSERR" 	       Transfer error, 0xf2
 *@li	\ref NDK_ERR_MI_WRITEERR "NDK_ERR_MI_WRITEERR" 	       Write error, 0xf1
 *@li	\ref NDK_ERR_MI_INCRERR "NDK_ERR_MI_INCRERR" 	       Increment error, 0xf0
 *@li	\ref NDK_ERR_MI_DECRERR "NDK_ERR_MI_DECRERR" 	        Decrement error,  0xef
 *@li	\ref NDK_ERR_MI_OVFLERR "NDK_ERR_MI_OVFLERR" 	       Overflow error, 0xed
 *@li	\ref NDK_ERR_MI_FRAMINGERR "NDK_ERR_MI_FRAMINGERR" 	   Framing error,  0xeb
 *@li	\ref NDK_ERR_MI_COLLERR "NDK_ERR_MI_COLLERR" 	       Collison detected,    0xe8
 *@li	\ref NDK_ERR_MI_INTERFACEERR "NDK_ERR_MI_INTERFACEERR"    Reset interface error,  0xe6
 *@li	\ref NDK_ERR_MI_ACCESSTIMEOUT "NDK_ERR_MI_ACCESSTIMEOUT" 	 Receive timeout,  0xe5
 *@li	\ref NDK_ERR_MI_PROTOCOLERR "NDK_ERR_MI_PROTOCOLERR" 	      Protocol error,  0xe4
 *@li	\ref NDK_ERR_MI_QUIT "NDK_ERR_MI_QUIT" 	                    Abnomal quit,   0xe2
 *@li	\ref NDK_ERR_MI_PPSErr "NDK_ERR_MI_PPSErr" 	             PPS error,  0xe1
 *@li	\ref NDK_ERR_MI_SpiRequest "NDK_ERR_MI_SpiRequest" 	       Request SPI error,  0xa0
 *@li	\ref NDK_ERR_MI_NY_IMPLEMENTED "NDK_ERR_MI_NY_IMPLEMENTED" 	   Unknown error, 0x9c
 *@li	\ref NDK_ERR_MI_CardTypeErr "NDK_ERR_MI_CardTypeErr" 	     Wrong card type, 0x83
 *@li	\ref NDK_ERR_MI_ParaErrInIoctl "NDK_ERR_MI_ParaErrInIoctl" 	 Invalid parameter for ioctl, 0x82
 *@li	\ref NDK_ERR_MI_Para "NDK_ERR_MI_Para"                    Invalid internal parameter, 0xa9
*/
int NDK_M1Transfer(uchar ucBlockNum);

/**
 *@brief	 Restore M1 card register to invalidate increment/decrement
 *@param[in] ucBlockNum:   Block to restore
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
 *@li	\ref NDK_ERR_MI_NOTAGERR "NDK_ERR_MI_NOTAGERR" 	No card,		0xff
 *@li	\ref NDK_ERR_MI_CRCERR "NDK_ERR_MI_CRCERR" 	CRC error,        0xfe
 *@li	\ref NDK_ERR_MI_EMPTY "NDK_ERR_MI_EMPTY" 	Not empty,          0xfd
 *@li	\ref NDK_ERR_MI_AUTHERR "NDK_ERR_MI_AUTHERR" 	Authentication error,   0xfc
 *@li	\ref NDK_ERR_MI_PARITYERR "NDK_ERR_MI_PARITYERR" 	Parity error,    oxfb
 *@li	\ref NDK_ERR_MI_CODEERR "NDK_ERR_MI_CODEERR" 		Received code error, 0xfa
 *@li	\ref NDK_ERR_MI_SERNRERR "NDK_ERR_MI_SERNRERR" 	  Anti collision data check error, 0xf8
 *@li	\ref NDK_ERR_MI_KEYERR "NDK_ERR_MI_KEYERR" 	       Key error, 0xf7
 *@li	\ref NDK_ERR_MI_NOTAUTHERR "NDK_ERR_MI_NOTAUTHERR" 	 Not authenticated,  0xf6
 *@li	\ref NDK_ERR_MI_BITCOUNTERR "NDK_ERR_MI_BITCOUNTERR" 	 Received bit error,  0xf5
 *@li	\ref NDK_ERR_MI_BYTECOUNTERR "NDK_ERR_MI_BYTECOUNTERR" 	  Received byte error, 0xf4
 *@li	\ref NDK_ERR_MI_WriteFifo "NDK_ERR_MI_WriteFifo" 	   FIFO write error, 0xf3
 *@li	\ref NDK_ERR_MI_TRANSERR "NDK_ERR_MI_TRANSERR" 	       Transfer error, 0xf2
 *@li	\ref NDK_ERR_MI_WRITEERR "NDK_ERR_MI_WRITEERR" 	       Write error, 0xf1
 *@li	\ref NDK_ERR_MI_INCRERR "NDK_ERR_MI_INCRERR" 	       Increment error, 0xf0
 *@li	\ref NDK_ERR_MI_DECRERR "NDK_ERR_MI_DECRERR" 	        Decrement error,  0xef
 *@li	\ref NDK_ERR_MI_OVFLERR "NDK_ERR_MI_OVFLERR" 	       Overflow error, 0xed
 *@li	\ref NDK_ERR_MI_FRAMINGERR "NDK_ERR_MI_FRAMINGERR" 	   Framing error,  0xeb
 *@li	\ref NDK_ERR_MI_COLLERR "NDK_ERR_MI_COLLERR" 	       Collison detected,    0xe8
 *@li	\ref NDK_ERR_MI_INTERFACEERR "NDK_ERR_MI_INTERFACEERR"    Reset interface error,  0xe6
 *@li	\ref NDK_ERR_MI_ACCESSTIMEOUT "NDK_ERR_MI_ACCESSTIMEOUT" 	 Receive timeout,  0xe5
 *@li	\ref NDK_ERR_MI_PROTOCOLERR "NDK_ERR_MI_PROTOCOLERR" 	      Protocol error,  0xe4
 *@li	\ref NDK_ERR_MI_QUIT "NDK_ERR_MI_QUIT" 	                    Abnomal quit,   0xe2
 *@li	\ref NDK_ERR_MI_PPSErr "NDK_ERR_MI_PPSErr" 	             PPS error,  0xe1
 *@li	\ref NDK_ERR_MI_SpiRequest "NDK_ERR_MI_SpiRequest" 	       Request SPI error,  0xa0
 *@li	\ref NDK_ERR_MI_NY_IMPLEMENTED "NDK_ERR_MI_NY_IMPLEMENTED" 	   Unknown error, 0x9c
 *@li	\ref NDK_ERR_MI_CardTypeErr "NDK_ERR_MI_CardTypeErr" 	     Wrong card type, 0x83
 *@li	\ref NDK_ERR_MI_ParaErrInIoctl "NDK_ERR_MI_ParaErrInIoctl" 	 Invalid parameter for ioctl, 0x82
 *@li	\ref NDK_ERR_MI_Para "NDK_ERR_MI_Para"                    Invalid internal parameter, 0xa9
*/
int NDK_M1Restore(uchar ucBlockNum);

/**
*@brief	Detect card rapidly
*@param[in] nModeCode	  0: Normal card seeking; Non-zero: Rapid card seeking
*@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR "NDK_ERR" 		Fail
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
*/
int NDK_PiccQuickRequest(int nModeCode);

/**
 *@brief	Set ISO1443-4 protocol support
 *@param[in] nModeCode	0: Not ignore; Non-zero: Ignore
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		        Failed to call driver function
*/
int NDK_SetIgnoreProtocol(int nModeCode);

/**
*@brief	Get ISO1443-4 protocol support
*@param[out]	pnModeCode	0: Not ignore; Non-zero: Ignore
*@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 	 	Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		           Failed to call driver function
*/
int NDK_GetIgnoreProtocol(int *pnModeCode);

/**
 *@brief	Read contactless reader chip type
 *@param[out]	 pnRfidType	Chip type (\ref EM_RFID "EM_RFID")
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 		 Failed to call driver function
*/
int  NDK_GetRfidType(int *pnRfidType);

/**
 *@brief    Detect Felica card
 *@param[out]   psRecebuf	Received data
 *@param[out]	pnRecvlen	Length of received data
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref  NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		 Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL " 		 Failed to call driver function
 *@li	\ref NDK_ERR_RFID_NOCARD "NDK_ERR_RFID_NOCARD" 		 No card
*/
int NDK_RfidFelicaPoll(uchar *psRecebuf, int *pnRecvlen);

/**
 *@brief    Perform APDU for Felica card
 *@param[in]	nSendlen	Length of send data
 *@param[in]   psSendbuf	Send data buffer
 *@param[out]   psRecebuf	Receive data buffer
 *@param[out]   pnRecvlen	Length of received data
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		Invalid parameter
 *@li	\ref  NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV" 		 Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL " 		 Failed to call driver function
*/
int  NDK_RfidFelicaApdu(int nSendlen, uchar *psSendbuf, int *pnRecvlen,  uchar *psRecebuf);

/** @}*/ // End of contactless

/** @addtogroup System
* @{
*/

/**
 *@brief  Get POS version and hardware info\n
   pinfo[0]: Wireless module\n
    0xFF   :No wireless module\n
    0x02    :MC39\n
    0x03    :SIM300\n
    0x06    :M72\n
    0x07    :BGS2\n
    0x08    :G610\n
	Most significant byte 0x80 indicate CDMA module\n
    0x81    :DTGS800\n
    0x82    :DTM228c\n
   pinfo[1]: RF module\n
    0xFF     : No RF module\n
    0x01     :RC531\n
    0x02    :PN512\n
   pinfo[2]: Magnetic card module\n
    0xFF     :No magnetic card module\n
    0x01     :Mesh\n
    0x02     :Giga\n
   pinfo[3]: Barcode scanner module\n
    0xFF     :No barcode scanner module\n
    0x01     :EM1300\n
    0x02     :EM3000\n
    0x03     :SE955\n
   pinfo[4]: External PinPad support\n
    0xFF     :NO\n
    0x01     :YES\n
   pinfo[5]: Number of serial ports\n
    0xFF     :No serial port\n
    0x01     :1\n
    0x02     :2\n
   pinfo[6]: USB support\n
    0xFF     :NO\n
    0x01     :YES\n
   pinfo[7]: Modem module\n
    0xFF     :NO\n
    0x01     :YES\n
   pinfo[8]: WiFi module\n
    0xFF     :No wifi module\n
    0x01     :"USI WM-G-MR-09"\n
   pinfo[9]: Ethernet support\n
    0xFF     :NO\n
    0x01     :dm9000\n
    0x02     :bcm589x core\n
   pinfo[10]: Printer module\n
    0xFF     : No printer module\n
    0x01~0x7F     :Thermal printer\n
    0x82~0xFF     :Dot matrix printer\n
   pinfo[11]: Touch screen support\n
    0xFF     :NO\n
    0x01:ts_2046\n
    0x02:589x_ts\n
   pinfo[12]: RF LED support\n
    0xFF     :NO\n
    0x01     :YES\n
   pinfo[13]: Bluetooth module\n
    0xFF     :NO\n
    0x01     :YES\n
   pinfo[14]: NFC module\n
    0xFF     :NO\n
    0x01     :YES\n
   pinfo[15]: Reserved\n
    0xFF     :NO\n
    0x01     :THK88\n
*/
typedef enum {
        SYS_HWINFO_GET_POS_TYPE = 0,     /**<Model*/
        SYS_HWINFO_GET_HARDWARE_INFO,    /**<All hardware info*/
        SYS_HWINFO_GET_BIOS_VER,         /**<OS version*/
        SYS_HWINFO_GET_POS_USN,          /**<USN.*/
        SYS_HWINFO_GET_POS_PSN,          /**<PSN*/
        SYS_HWINFO_GET_BOARD_VER,        /**<Mainboard version*/
        SYS_HWINFO_GET_CREDITCARD_COUNT, /**<Total number of card swiped*/
        SYS_HWINFO_GET_PRN_LEN,          /**<Total print length*/
        SYS_HWINFO_GET_POS_RUNTIME,      /**<Total run time from power up*/
        SYS_HWINFO_GET_KEY_COUNT,        /**<Total number of key pressed*/
        SYS_HWINFO_GET_CPU_TYPE,         /**<CPU type*/
        SYS_HWINFO_GET_BOOT_VER,         /**<Boot version*/
        SYS_HWINFO_GET_PATCH_VER,        /**<Patch version*/
        SYS_HWINFO_GET_PUBKEY_VER,       /**<Public key version*/
	SYS_HWINFO_GET_POS_TID,          /**<TID in 11 digital numbers**/
} EM_SYS_HWINFO;

/**
 *@brief  Configuration type index
*/
typedef enum {
        SYS_CONFIG_SLEEP_ENABLE, /**<0: Disable sleep, 1:Enable sleep*/
        SYS_CONFIG_SLEEP_TIME,   /**<Idle time before entering sleep*/
        SYS_CONFIG_SLEEP_MODE,   /**<1: Shallow sleep 2: Deep sleep*/
        SYS_CONFIG_LANGUAGE,     /**<0: Chinese, 1: English*/
        SYS_CONFIG_APP_AUTORUN,  /**<0: Disable auto-run application 1: Enabled auto-run application*/
} EM_SYS_CONFIG;

/**
 *@brief  Hardware types
*/
typedef enum {
        SYS_HWTYPE_WIRELESS_MODEM = 0, /**<Wireless modem*/
        SYS_HWTYPE_RFID,             /**<Contactless*/
        SYS_HWTYPE_MAG_CARD,         /**<Magnetic card*/
        SYS_HWTYPE_SCANNER,          /**<Barcode scanner*/
        SYS_HWTYPE_PINPAD,           /**<External PinPad*/
        SYS_HWTYPE_AUX,              /**<Serial port*/
        SYS_HWTYPE_USB,              /**<USB*/
        SYS_HWTYPE_MODEM,            /**<Modem*/
        SYS_HWTYPE_WIFI,             /**<WiFi*/
        SYS_HWTYPE_ETHERNET,         /**<Ethernet*/
        SYS_HWTYPE_PRINTER,          /**<Printer*/
        SYS_HWTYPE_TOUCHSCREEN,      /**<Touch screen*/
        SYS_HWTYPE_RFIDLED,          /**<Contactless LED*/
        SYS_HWTYPE_BT,               /**<Bluetooth*/
        SYS_HWTYPE_NFC,              /**<NFC*/
        SYS_HWTYPE_GM,
        SYS_HWTYPE_MAX
} EM_SYS_HWTYPE;

/**
 *@brief LED control
*/
typedef enum {
        LED_RFID_RED_ON       = 0x01,  /**<Red on*/
        LED_RFID_RED_OFF      = 0x02,  /**<Red off*/
        LED_RFID_RED_FLICK    = 0x03,  /**<Red blink*/
        LED_RFID_YELLOW_ON    = 0x04,  /**<Yellow on*/
        LED_RFID_YELLOW_OFF   = 0x08,  /**<Yellow off*/
        LED_RFID_YELLOW_FLICK = 0x0c,  /**<Yellow blink*/
        LED_RFID_GREEN_ON     = 0x10,  /**<Green on*/
        LED_RFID_GREEN_OFF    = 0x20,  /**<Green off*/
        LED_RFID_GREEN_FLICK  = 0x30,  /**<Green blink*/
        LED_RFID_BLUE_ON      = 0x40,  /**<Blue on*/
        LED_RFID_BLUE_OFF     = 0x80,  /**<Blue off*/
        LED_RFID_BLUE_FLICK   = 0xc0,  /**<Blue blink*/
        LED_COM_ON            = 0x100, /**<Communication led on*/
        LED_COM_OFF           = 0x200, /**<Communication led off*/
        LED_COM_FLICK         = 0x300, /**<Communication led blink*/
        LED_ONL_ON            = 0x400, /**<Online led on*/
        LED_ONL_OFF           = 0x800, /**<Online led off*/
        LED_ONL_FLICK         = 0xc00  /**<Online led blink*/
} EM_LED;

/**
 *@brief Device types for statistics
*/
typedef enum {
        SS_TYPE_KEYBOARD,    /**<Keypad*/
        SS_TYPE_PRINTER,     /**<Printer*/
        SS_TYPE_MAG,         /**<Magnetic card*/
        SS_TYPE_ICCARD,      /**<Smart card*/
        SS_TYPE_RFID,        /**<Contactless card*/
        SS_TYPE_MODEM,       /**<Modem*/
        SS_TYPE_WLS,         /**<Wireless*/
        SS_TYPE_WIFI,        /**<WiFi*/
        SS_TYPE_POWER,       /**<Power*/
        SS_TYPE_DEV_MAIN_NUM /**<Number of devices*/
} EM_SS_TYPE;

/**
 *@brief Device ID for statistics
*/
typedef enum {
        SS_KEYBOARD_ZERO      = (SS_TYPE_KEYBOARD << 16 | 13),
        SS_KEYBOARD_ONE       = (SS_TYPE_KEYBOARD << 16 | 26),
        SS_KEYBOARD_TWO       = (SS_TYPE_KEYBOARD << 16 | 25),
        SS_KEYBOARD_THREE     = (SS_TYPE_KEYBOARD << 16 | 24),
        SS_KEYBOARD_FOUR      = (SS_TYPE_KEYBOARD << 16 | 22),
        SS_KEYBOARD_FIVE      = (SS_TYPE_KEYBOARD << 16 | 21),
        SS_KEYBOARD_SIX       = (SS_TYPE_KEYBOARD << 16 | 20),
        SS_KEYBOARD_SEVEN     = (SS_TYPE_KEYBOARD << 16 | 18),
        SS_KEYBOARD_EIGHT     = (SS_TYPE_KEYBOARD << 16 | 17),
        SS_KEYBOARD_NINE      = (SS_TYPE_KEYBOARD << 16 | 16),

        SS_KEYBOARD_ENTER     = (SS_TYPE_KEYBOARD << 16 | 8),
        SS_KEYBOARD_ESC       = (SS_TYPE_KEYBOARD << 16 | 10),
        SS_KEYBOARD_F2        = (SS_TYPE_KEYBOARD << 16 | 29),
        SS_KEYBOARD_F1        = (SS_TYPE_KEYBOARD << 16 | 28),
        SS_KEYBOARD_DOT       = (SS_TYPE_KEYBOARD << 16 | 14),
        SS_KEYBOARD_ZMK       = (SS_TYPE_KEYBOARD << 16 | 12), /**<Letter key*/
        SS_KEYBOARD_F3        = (SS_TYPE_KEYBOARD << 16 | 30),
        SS_KEYBOARD_BASP      = (SS_TYPE_KEYBOARD << 16 | 9), /**<Backspace key*/
        SS_KEYBOARD_0_ID      = (SS_TYPE_KEYBOARD << 16 | 2), /**<Other keys*/
        SS_KEYBOARD_1_ID      = (SS_TYPE_KEYBOARD << 16 | 3),
        SS_KEYBOARD_2_ID      = (SS_TYPE_KEYBOARD << 16 | 4),
        SS_KEYBOARD_3_ID      = (SS_TYPE_KEYBOARD << 16 | 5),
        SS_KEYBOARD_F4        = (SS_TYPE_KEYBOARD << 16 | 31),
        SS_KEYBOARD_TOTAL     = (SS_TYPE_KEYBOARD << 16 | 33), /**<All keys*/

        SS_PRT_PAPER_ID       = (SS_TYPE_PRINTER << 16 | 0), /**<Print length in meters*/
        SS_PRT_HEAT_ID        = (SS_TYPE_PRINTER << 16 | 1), /**<Thermal heating time in milliseconds*/
        SS_PRT_STITCH_ID      = (SS_TYPE_PRINTER << 16 | 2),

        SS_MAG_TIMES_ID       = (SS_TYPE_MAG << 16 | 0),   /**<Number of magnetic card swiped*/

        SS_ICCARD_BASE_ID     = (SS_TYPE_ICCARD << 16 | 0), /**<Number of smart card inserted*/

        SS_RFID_TIMES_ID      = (SS_TYPE_RFID << 16 | 0),  /**<Number of RF card seeked*/

        SS_MODEM_TIMES_ID     = (SS_TYPE_MODEM << 16 | 0), /**<Modem connection times*/
        SS_MODEM_FAILTIMES_ID = (SS_TYPE_MODEM << 16 | 1), /**<Modem connection failure times*/
        SS_MODEM_SDLCTIME_ID  = (SS_TYPE_MODEM << 16 | 2), /**<Synchronous connection duration in milliseconds*/
        SS_MODEM_ASYNTIME_ID  = (SS_TYPE_MODEM << 16 | 3), /**<Asynchronous connection duration for asynchronous in milliseconds*/

        SS_WLS_TIMES_ID       = (SS_TYPE_WLS << 16 | 0),   /**<Wireless connection times*/
        SS_WLS_FAILTIMES_ID   = (SS_TYPE_WLS << 16 | 1),   /**<Wireless connection failure times*/
        SS_WLS_PPPTIME_ID     = (SS_TYPE_WLS << 16 | 2),   /**<PPP connection duration in milliseconds*/

        SS_WIFI_TIMES_ID      = (SS_TYPE_WIFI << 16 | 0),  /**<WiFi connection times*/
        SS_WIFI_TIME_ID       = (SS_TYPE_WIFI << 16 | 1),  /**<WiFi connection duration in milliseconds*/

        SS_POWER_TIMES_ID     = (SS_TYPE_POWER << 16 | 0), /**<Power on/off times*/
        SS_POWERUP_TIME_ID    = (SS_TYPE_POWER << 16 | 1)  /**<Run time from power on*/
} EM_SS_DEV_ID;

/**
 *@brief Firmware types
*/
typedef enum {
        SYS_FWINFO_PRO, /**<Production version*/
        SYS_FWINFO_DEV  /**<Development version*/
} EM_SYS_FWINFO;

/**
 *@brief Power configuration
*/
typedef struct __POWERINFO {
        uchar unIsCharging;    /**<0: Not charging; 1: Charging; 0xff: Unknown*/
        int unPowerType;       /**<Power type\n Bit0: 1:External adapter\n Bit1: 1: Battery\n Bit2: 1: USB powered; 0xff: Unknown*/
        uchar unBattryPercent; /**<Battery percentage (0-100). 0: Externl adapter, 0xff: Unknown*/
} ST_POWER_INFO;

/**
 *@brief LED flicker control
*/
typedef struct {
        uint unFlickOn;   /**<Flickering on duration (Unit:100 ms)*/
        uint unFlickOff;  /**<Flickering off duration (Unit:100 ms)*/
        uint unRverse[2];
} ST_LED_FLICK;

/**
 *@brief	Get API version
 *@param[out]   pszVer	Version string (No less than 16 bytes)
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_Getlibver(char *pszVer);

/**
 *@brief 		Initialize system (Depreciated)
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail

*/
int NDK_SysInit(void);

/**
 *@brief 		Exit system
 *@param[in]   nErrCode	 Error code returned when exit. (0: Normal exit; Non-zero: Abnormal exit)
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail

*/
int NDK_SysExit(int nErrCode);

/**
 *@brief 		Restart terminal
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_SysReboot(void);

/**
 *@brief 		Shutdown terminal
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_SysShutDown(void);

/**
 *@brief 		Beep once
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV"		Failed to open device
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL"			     Failed to call driver function
*/
int NDK_SysBeep(void);

/**
 *@brief 		Set beep volume
 *@param[in]   unVolNum    Volume value (0-5, Default:5)
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"		Invalid parameter
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV"		Failed to open device
*/
int NDK_SysSetBeepVol(uint unVolNum);

/**
 *@brief 		Get beep volume
 *@param[out]    punVolNum    Volume value
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	 \ref NDK_ERR_PARA "NDK_ERR_PARA"		Invalid parameter
 *@li	 \ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV"		Failed to open device
*/
int NDK_SysGetBeepVol(uint *punVolNum);

/**
 *@brief 		Beep
 *@param[in]   unFrequency  Frequency in Hz (0-4000)
 *@param[in]   unMsSeconds Duration in ms
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	 \ref NDK_ERR_PARA "NDK_ERR_PARA"		Invalid parameter
 *@li	 \ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV"		Failed to open device
 *@li	 \ref NDK_ERR_IOCTL "NDK_ERR_IOCTL" 			Failed to call driver function
*/
int NDK_SysTimeBeep(uint unFrequency, uint unMsSeconds);

/**
 *@brief 		Enable/Disable auto-sleep
 *@param[in]   unFlag  0: Disable; 1: Enable
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	 \ref NDK_ERR_PARA "NDK_ERR_PARA"			Invalid parameter
 *@li	 \ref NDK_ERR "NDK_ERR"	                      Fail
*/
int NDK_SysSetSuspend(uint unFlag);

/**
 *@brief 		Enter sleep mode immediately
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	 \ref NDK_ERR_RFID_BUSY "NDK_ERR_RFID_BUSY"	 RF busy
 *@li	 \ref NDK_ERR_PRN_BUSY "NDK_ERR_PRN_BUSY"	 Printer busy
 *@li	 \ref NDK_ERR_ICCARD_BUSY "NDK_ERR_ICCARD_BUSY"	 Smart card busy
 *@li	 \ref NDK_ERR_MAG_BUSY "NDK_ERR_MAG_BUSY"	   Magnetic card busy
 *@li	 \ref NDK_ERR_USB_BUSY "NDK_ERR_USB_BUSY"	  USB busy
 *@li	 \ref NDK_ERR_WLM_BUSY "NDK_ERR_WLM_BUSY"	  Wireless module busy
 *@li	 \ref NDK_ERR_PIN_BUSY "NDK_ERR_PIN_BUSY"	 PIN input state
 *@li	 \ref NDK_ERR "NDK_ERR"	                    Fail
*/
int NDK_SysGoSuspend(void);

/**
 *@brief 		Get battery level
 *@param[out]   punVol  0: External adapter; Non-zero: Battery level
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	 \ref NDK_ERR_PARA "NDK_ERR_PARA"		Invalid parameter
 *@li	 \ref NDK_ERR "NDK_ERR"	                Fail
*/
int NDK_SysGetPowerVol(uint *punVol);

/**
 *@brief 		Delay
 *@param[in]   unDelayTime Delay time (Unit: 0.1s)
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	 \ref NDK_ERR_PARA "NDK_ERR_PARA"		Invalid parameter
*/
int NDK_SysDelay(uint unDelayTime);

/**
 *@brief 		Delay (Unit: 1ms)
 *@param[in]   unDelayTime Delay time
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	 \ref NDK_ERR_PARA "NDK_ERR_PARA"		Invalid parameter
*/
int NDK_SysMsDelay(uint unDelayTime);

/**
 *@brief 		Get time
 *@param[out]    pstTime Current time
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	 \ref NDK_ERR_PARA "NDK_ERR_PARA"		Invalid parameter
*/
int NDK_SysGetPosTime(struct tm *pstTime);

/**
 *@brief 		Set time
 *@param[in]    stTime Time
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	 \ref NDK_ERR_PARA "NDK_ERR_PARA"		Invalid parameter
 *@li	 \ref NDK_ERR "NDK_ERR"		Fail
*/
int NDK_SysSetPosTime(struct tm stTime);

/**
 *@brief 	  Get font data (Not implemented)
 *@param[out]   psPath Path of font library
 *@param[in]    unOffSet Offset of character in font library
 *@param[out]   psBuf Buffer to save data returned
 *@param[in]    unLen  length  of read data
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_SysReadFont(const char * psPath, uint unOffSet, char *psBuf, uint unLen);

/**
 *@brief 		Turn on/off LED
 *@param[in]   emStatus   	Use '|' to control more than one LEDs. e.g. NDK_LedStatus(LED_RFID_RED_ON|LED_RFID_YELLOW_FLICK) means turning red LED on and flicker yellow LED,  other LEDs status remain unchanged.
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	 \ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
 *@li	 \ref NDK_ERR "NDK_ERR"   		 Fail
*/
int NDK_LedStatus(EM_LED emStatus);

/**
 *@brief 		Set flickering parameter
 *@param[in]   emStatus   Only valid for flickering mode
 *@param[in] 	stFlickParam  Flicker parameter
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li  \ref NDK_ERR_PARA  "NDK_ERR_PARA"     Invalid parameter
 *@li  \ref NDK_ERR       "NDK_ERR"          Fail
*/
int NDK_LedSetFlickParam(EM_LED emStatus, ST_LED_FLICK stFlickParam);

/**
 *@brief 		Start watch
 *@details  Usually for calculating time elapsed, work with \ref NDK_SysStopWatch "NDK_SysStopWatch". (Accuracy within 1 millisecond.)
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
*/
int NDK_SysStartWatch(void);

/**
 *@brief 		Stop watch
 *@param[out]   punTime Time count value
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	 \ref NDK_ERR "NDK_ERR"		Fail
*/
int NDK_SysStopWatch(uint *punTime);

/**
 *@brief 		Get hardware info
 *@param[in]   emFlag Hardware type
 *@param[out]   punLen Length of buffer returned (16-100)
 *@param[out]   psBuf Buffer to save data returned
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	 \ref NDK_ERR_PARA "NDK_ERR_PARA"   Invalid parameter
 *@li	 \ref NDK_ERR "NDK_ERR"		    Fail
*/
int NDK_SysGetPosInfo(EM_SYS_HWINFO emFlag, uint *punLen, char *psBuf);

/**
 *@brief    Get system configuration
 *@param[in]   emConfig Configuration type index
 *@param[out]   pnValue Value of configuration
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	 \ref NDK_ERR_PARA "NDK_ERR_PARA"	 Invalid parameter
 *@li	 \ref NDK_ERR "NDK_ERR"	           Fail
*/
int NDK_SysGetConfigInfo(EM_SYS_CONFIG emConfig, int *pnValue);

/**
 *@brief	 Initialize statistics data
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li   \ref NDK_ERR "NDK_ERR"			Fail
*/
int NDK_SysInitStatisticsData(void);

/**
 *@brief       Get statistics data
 *@param[in] 	emDevId 	Device ID (\ref EM_SS_DEV_ID "EM_SS_DEV_ID")
 *@param[out]   pulValue 	Statistics data
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	 \ref NDK_ERR "NDK_ERR"	   Fail
 *@li	 \ref NDK_ERR_PARA "NDK_ERR_PARA"	 Invalid parameter
*/
int NDK_SysGetStatisticsData(EM_SS_DEV_ID emDevId, ulong *pulValue);

/**
 *@brief    Get firmware type
 *@param[out]  	emFWinfo 	Firmware type (\ref EM_SYS_FWINFO "EM_SYS_FWINFO")
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	 \ref NDK_ERR_PARA "NDK_ERR_PARA"	 Invalid parameter
*/
int NDK_SysGetFirmwareInfo(EM_SYS_FWINFO *emFWinfo);

/**
 *@brief	 Get time from 1970-01-01 00:00:00 in seconds
 *@param[out]   ulTime 	Time in seconds
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	 \ref NDK_ERR_PARA "NDK_ERR_PARA"	 Invalid parameter
*/
int NDK_SysTime(ulong *ulTime);

/**
 *@brief   Set auto-wakeup
 *@param[in]  unSec         Wakeup time in seconds (No less than 60 seconds)
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	      Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR"	               Fail
*/
int NDK_SysSetSuspendDuration(uint unSec);

/**
 *@brief    Get power info
 *@param[in]  pstPowerInfo   Power info (\ref ST_POWER_INFO "ST_POWER_INFO")
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li    \ref NDK_ERR    "NDK_ERR"   Fail
 *@li    \ref NDK_ERR_PARA   "NDK_ERR_PARA"     Invalid parameter
*/
int NDK_SysGetPowerInfo(ST_POWER_INFO *pstPowerInfo);

int NDK_SysCallBackOnShutDown(int (*callback)(void) );

/** @}*/ // End of system

/** @addtogroup  Utilities
* @{
*/

/**
 *@brief	Add two unsigned number
 *@details Add two Numbers of successive bit by bit (The length of combined results should not be more than 12)
 *@param[in] pszDigStr1 First number string to add
 *@param[in] pszDigStr2 Second number string to add
 *@param[out]	pszResult Result data
 *@param[out]	pnResultLen Result data length
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"		Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR"		 Fail
*/
int NDK_AddDigitStr(const uchar *pszDigStr1, const uchar *pszDigStr2, uchar* pszResult, int *pnResultLen );

/**
 *@brief	 Increase 6-bit numeric string
 *@param[in,out] pszStrNum		Numeric string
 *@return
 *@li	\ref NDK_OK "NDK_OK"  					Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"		Invalid parameter
*/
int NDK_IncNum (uchar * pszStrNum );

/**
 *@brief	Remove decimal point in amount string
 *@param[in] pszSource		Amount string to convert
 *@param[in] pnTargetLen		Size of destination buffer
 *@param[out]	pszTarget		String converted
 *@param[out]	pnTargetLen		Length of string converted
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
*/
int NDK_FmtAmtStr (const uchar* pszSource, uchar* pszTarget, int* pnTargetLen );

/**
 *@brief	 Convert ASCII string to HEX
 *@details	If the length of strinig is not even, use "0" as a fill character to the left and "F" to the right per alignment method
 *@param[in] pszAsciiBuf		ASCII string to convert
 *@param[in] nLen			Length of input data
 *@param[in] ucType			Alignment (0: Left aligned; 1: Right aligned)
 *@param[out]	psBcdBuf		HEX data output from conversion
 *@return
 *@li	\ref NDK_OK "NDK_OK"  					Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
*/
int NDK_AscToHex (const uchar* pszAsciiBuf, int nLen, uchar ucType, uchar* psBcdBuf);

/**
 *@brief	 Convert HEX data to ASCII string
 *@param[in] psBcdBuf		Hex data to convert
 *@param[in] nLen			Length of data converted
 *@param[in] ucType			Alignment (0: Left aligned; 1: Right aligned)
 *@param[out]	pszAsciiBuf		ASCII data output from conversion
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
*/
int NDK_HexToAsc (const uchar* psBcdBuf, int nLen, uchar ucType, uchar* pszAsciiBuf);

/**
 *@brief 	Convert integer to 4-byte character array (High byte first)
 *@param[in] unNum		Integer to convert
 *@param[out]	psBuf		String output from conversion
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
*/
int NDK_IntToC4 (uchar* psBuf, uint unNum );

/**
 *@brief	 Convert integer to 2-byte character array (High byte first)
 *@param[in] unNum		Integer to convert
 *@param[out]	psBuf		String output from conversion
 *@return
 *@li	\ref NDK_OK "NDK_OK"  					Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
*/
int NDK_IntToC2(uchar* psBuf, uint unNum );

/**
 *@brief	 Convert 4-byte character array to integer
 *@param[in] psBuf		String to convert
 *@param[out]	unNum		Integer output from conversion
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
*/
int NDK_C4ToInt(uint* unNum, uchar* psBuf );

/**
 *@brief	Convert 2-byte character array to integer
 *@param[in] psBuf		String to convert, length should be more than 2
 *@param[out]	unNum		Integer output from conversion
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_C2ToInt(uint *unNum, uchar *psBuf);

/**
 *@brief	 Convert integer (0-99) to 1-byte BCD
 *@param[in] nNum		Integer (0-99) to convert
 *@param[out]	psCh			1-byte BCD character output from conversion
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
*/
int NDK_ByteToBcd(int nNum, uchar *psCh);

/**
 *@brief	 Convert 1-byte BCD to integer (0-99)
 *@param[in] ucCh		BCD character to convert
 *@param[out]	pnNum	Integer value (0-99) output from conversion
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
*/
int NDK_BcdToByte(uchar ucCh, int *pnNum);

/**
 *@brief	 Convert integer (0-9999)  to 2-byte BCD
 *@param[in] nNum		Numeric string 1
 *@param[in] pnBcdLen	Size of output buffer
 *@param[out]	pnBcdLen	Length of BCD converted
 *@param[out]	psBcd		2-byte BCD output from conversion
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
*/
int NDK_IntToBcd(uchar *psBcd, int *pnBcdLen, int nNum);

/**
 *@brief	Convert 2-byte BCD to integer (0-9999)
 *@param[in] psBcd		2-byte BCD to convert
 *@param[out]	nNum		Integer (0-9999) converted to
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
*/
int NDK_BcdToInt(const uchar * psBcd, int *nNum);

/**
 *@brief 	Calculte LRC
 *@param[in] psBuf		String that requires LRC calculation (Length should be more then nLen)
 *@param[in] nLen		Length of string that requires LRC calculation
 *@param[out]	ucLRC		LRC from calculation
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
*/
int NDK_CalcLRC(const uchar *psBuf, int nLen, uchar *ucLRC);

/**
 *@brief	 Trim whitespace from left end of string
 *@param[in] pszBuf		Buffer to store string
 *@param[out]	pszBuf		String with left end whitespace trimmed
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
*/
int NDK_LeftTrim(uchar *pszBuf);

/**
 *@brief	 Trim whitespace from right end of string
 *@param[in] pszBuf		Buffer to store string
 *@param[out]	pszBuf		String with right-end whitespace trimmed
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter

*/
int NDK_RightTrim(uchar *pszBuf);

/**
 *@brief	Trim whitespace from both left and right ends of string
 *@param[in] pszBuf			Buffer to store string
 *@param[out]	pszBuf			String with left and right end whitespace trimmed

 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter

*/
int NDK_AllTrim(uchar *pszBuf);

/**
 *@brief	 Pad string
 *@param[in] pszString		Buffer to store string
 *@param[in]   nLen			Length of string
 *@param[in] ucCh			Character to add
 *@param[in] nOption			Operation types\n
                          	0    Pad character to the head of string\n
                          	1    Pad character to the end of string\n
                          	2    Pad character to both the head and end of string
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
*/
int NDK_AddSymbolToStr(uchar *pszString, int nLen, uchar ucCh, int nOption);

/**
 *@brief	Get substring
 *@param[in] pszSouStr		Source string
 *@param[in] nStartPos		Position to get substring (Starting from 1)
 *@param[in] nNum			Number of characters to get
 *@param[out]	pszObjStr		Buffer to store substring end with '\0'
 *@param[out]	pnObjStrLen		Length of substring
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
*/
int NDK_SubStr(const uchar *pszSouStr, int nStartPos, int nNum, uchar *pszObjStr, int *pnObjStrLen);

/**
 *@brief	Check if digital character
 *@param[in] ucCh	Character to check
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Digital number
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Not digital number
*/
int NDK_IsDigitChar(uchar ucCh);

/**
 *@brief 	Check if digital string
 *@param[in] pszString		String to be checked
 *@return
 *@li	\ref NDK_OK "NDK_OK"  		Digital string
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Not digital string
*/
int NDK_IsDigitStr(const uchar *pszString);

/**
 *@brief	Check if leap year
 *@param[in] nYear		Year
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Leap year
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Not leap year
*/
int NDK_IsLeapYear(int nYear);

/**
 *@brief	 Find maximum number of days in a given month of a year
 *@param[in] nYear		Year
 *@param[in] nMon		Month
 *@param[out]	pnDays		Maximum number of days in that month of that year
 *@return
 *@li	\ref NDK_OK "NDK_OK"  					Success
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
*/
int NDK_MonthDays(int nYear, int nMon, int *pnDays);

/**
 *@brief	 Check if date string
 *@param[in] pszDate		Date string: YYYYMMDD
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Valid date string
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid data string
*/
int NDK_IsValidDate(const uchar *pszDate);

/** @}*/ //  End of utilities

/** @addtogroup ExternalStorage
* @{
*/

/**
 *@brief External storage types
*/
typedef enum {
        UDISK  = 0, /**<USB stick*/
        SDDISK = 1  /**<SD card*/
} EM_DISKTYPE;

/**
 *@brief Device status
*/
typedef enum {
        DISKMOUNTSUCC      = 1, /**<Available*/
        DISKNOTOPEN        = 2, /**<Not open*/
        DISKDRIVERLOADFAIL = 3, /**<Driver load error*/
        DISKMOUNTING       = 4, /**<Being loaded*/
        DISKNOEXIT         = 5, /**<Not detected*/
        DISKTIMEOUT        = 6  /**<Timeout*/
} EM_DISKSTATE;

typedef struct {
        uint unFreeSpace;  /**<Free space*/
        uint unTotalSpace; /**<Total size*/
} ST_DISK_INFO;

/**
 *@brief	Open device
 *@param[in] emType	Type (\ref EM_DISKTYPE "EM_DISKTYPE")
 *@param[in] nTimeOut Timeout in seconds (0: Block)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  					Success
 *@li	\ref NDK_ERR_USDDISK_PARAM "NDK_ERR_USDDISK_PARAM"	Invalid parameter
 *@li	\ref NDK_ERR_USDDISK_NONSUPPORTTYPE "NDK_ERR_USDDISK_NONSUPPORTTYPE"		Type not supported
 *@li	\ref NDK_ERR_USDDISK_DRIVELOADFAIL "NDK_ERR_USDDISK_DRIVELOADFAIL"	Failed to load driver
 *@li	\ref NDK_ERR_USDDISK_IOCFAIL "NDK_ERR_USDDISK_IOCFAIL"	 Failed to call driver function
*/
int NDK_DiskOpen(EM_DISKTYPE emType, int nTimeOut);

/**
 *@brief	Get info
 *@param[in] pszDiskDir	Directory mounted
 *@param[in] pstInfo   Disk info (\ref ST_DISK_INFO "ST_DISK_INFO")
 *@return
 *@li	\ref NDK_OK "NDK_OK"  					Success
 *@li	\ref NDK_ERR_USDDISK_PARAM "NDK_ERR_USDDISK_PARAM"	Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR"	 Fail
*/
int NDK_DiskGetInfo(char *pszDiskDir, ST_DISK_INFO  *pstInfo);

/**
 *@brief	Get state
 *@param[in] emType	Type (\ref EM_DISKTYPE "EM_DISKTYPE")
 *@param[out]	pnDiskState   Disk state (\ref EM_DISKSTATE "EM_DISKSTATE")
 *@return
 *@li	\ref NDK_OK "NDK_OK"  					Success
 *@li	\ref NDK_ERR_USDDISK_PARAM "NDK_ERR_USDDISK_PARAM"	  Invalid parameter
 *@li	\ref NDK_ERR_USDDISK_NONSUPPORTTYPE "NDK_ERR_USDDISK_NONSUPPORTTYPE"		Type not supported
*/
int NDK_DiskGetState(EM_DISKTYPE emType, int *pnDiskState);

/**
 *@brief	Close device
 *@param[in] emType	Type (\ref EM_DISKTYPE "EM_DISKTYPE")
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_USDDISK_PARAM "NDK_ERR_USDDISK_PARAM"	Invalid parameter
 *@li	\ref NDK_ERR_USDDISK_UNMOUNTFAIL "NDK_ERR_USDDISK_UNMOUNTFAIL"	Failed to unmount
 *@li	\ref NDK_ERR_USDDISK_UNLOADDRIFAIL "NDK_ERR_USDDISK_UNLOADDRIFAIL"	Failed to unload driver
*/
int NDK_DiskClose(EM_DISKTYPE emType);

/**
 *@brief	Get root directory
 *@param[in] emType	Type (\ref EM_DISKTYPE "EM_DISKTYPE")
 *@param[out]	pszRdir  Root directory
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_USDDISK_PARAM "NDK_ERR_USDDISK_PARAM"	  Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR"	  Fail
*@li	\ref NDK_ERR_USDDISK_NONSUPPORTTYPE "NDK_ERR_USDDISK_NONSUPPORTTYPE"	Type not supported
*/
int NDK_DiskGetRootDirName(EM_DISKTYPE emType, char **pszRdir);

/** @}*/ // End of external storage

/** @addtogroup WiFi
* @{
*/

/**
 *@brief Security mode
*/
typedef enum {
        WIFI_NET_SEC_NONE,       /**<No encryption*/
        WIFI_NET_SEC_WEP_OPEN,   /**<Open WEP*/
        WIFI_NET_SEC_WEP_SHARED, /**<Shared WEP*/
        WIFI_NET_SEC_WPA,        /**<WPA*/
        WIFI_NET_SEC_WPA2        /**<WPA2*/
} EM_WIFI_NET_SEC;

/**
 *@brief Password format
*/
typedef enum {
        WIFI_KEY_TYPE_NOKEY, /**<No password*/
        WIFI_KEY_TYPE_HEX,   /**<Heximal format*/
        WIFI_KEY_TYPE_ASCII  /**<ASCII format*/
} EM_WIFI_KEY_TYPE;

typedef struct {
        uchar ucIfDHCP;             /**<Enable DHCP or not*/
        EM_WIFI_KEY_TYPE emKeyType; /**<Password format*/
        EM_WIFI_NET_SEC emSecMode;  /**<Encryption mode*/
        char *pszKey;               /**<Password*/
        char *psEthIp;              /**<IP address*/
        char *psEthNetmask;         /**<Netmask address*/
        char *psEthGateway;         /**<Gateway address*/
        char *psEthDnsPrimary;      /**<First DNS address*/
        char *psEthDnsSecondary;    /**<Second DNS address*/
} ST_WIFI_PARAM;

/**
 *@brief Connection status
*/
typedef enum {
        WIFI_WPA_CONSTATE_UNLINKED = -1, /**<Not connected*/
        WIFI_WPA_CONSTATE_LINKING,       /**<Attempt to connect*/
        WIFI_WPA_CONSTATE_LINKED,        /**<Connected*/
        WIFI_WPA_CONSTATE_AUTHENTICATED, /**<Authenticated*/
        WIFI_WPA_CONSTATE_CONTEXT        /**<IP, netmask, gateway assigned*/
} EM_WPA_CONSTATE;

#define WIFI_IW_ESSID_MAX_SIZE     128 /**<Maximum SSID name length*/
#define WIFI_IW_ENCODING_TOKEN_MAX 128 /**<Maximum password length*/

typedef struct {
        char sEssid[WIFI_IW_ESSID_MAX_SIZE + 1];   /**<SSID name*/
        char sKeyModeStr[128];                     /**<Encryption mode*/
        int nFrequency;                            /**<Channel frequency*/
        char sKey[WIFI_IW_ENCODING_TOKEN_MAX + 1]; /**<Password*/
        char sSignal[16];                          /**<Signal strength*/
} ST_WIFI_APINFO;

/**
 *@brief	Inititialize WiFi
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_WIFI_DEVICE_FAULT "NDK_ERR_WIFI_DEVICE_FAULT"	Device fault
 *@li	\ref NDK_ERR_WIFI_CMD_UNSUPPORTED "NDK_ERR_WIFI_CMD_UNSUPPORTED"	Command not supported
 *@li	\ref NDK_ERR_WIFI_SEARCH_FAULT "NDK_ERR_WIFI_SEARCH_FAULT"	Scan error
 *@li	\ref NDK_ERR_WIFI_DEVICE_UNAVAILABLE "NDK_ERR_WIFI_DEVICE_UNAVAILABLE"	Device unavailable
*/
int NDK_WiFiInit(void);

/**
 *@brief	Get scanned SSIDs
 *@param[out]	ppszESSIDlist	SSID list
 *@param[out]	pnNumList   Number of SSIDs
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_WIFI_INVDATA "NDK_ERR_WIFI_INVDATA"	Invalid parameter
 *@li	\ref NDK_ERR_WIFI_DEVICE_FAULT "NDK_ERR_WIFI_DEVICE_FAULT"	Device fault
*/
int NDK_WiFiGetNetList(char **ppszESSIDlist, int *pnNumList);

/**
 *@brief	Get signal strength
 *@param[in] pszNetName	SSID name
 *@param[out]	pnSignal   Signal strength
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_WIFI_INVDATA "NDK_ERR_WIFI_INVDATA"	Invalid parameter
 *@li	\ref NDK_ERR_WIFI_DEVICE_NOTOPEN "NDK_ERR_WIFI_DEVICE_NOTOPEN"	SSID not found
 *@li	\ref NDK_ERR_WIFI_DEVICE_TIMEOUT "NDK_ERR_WIFI_DEVICE_TIMEOUT"	Timeout error
 *@li	\ref NDK_ERR_WIFI_DEVICE_FAULT "NDK_ERR_WIFI_DEVICE_FAULT"	Device fault
*/
int NDK_WiFiSignalCover(const char *pszNetName, int *pnSignal);

/**
 *@brief	Get security mode
 *@param[in] pszESSIDName	SSID name
 *@param[out]	pemSec	Security mode
 *@return
 *@li	\ref NDK_OK "NDK_OK"  					Success
 *@li	\ref NDK_ERR_WIFI_INVDATA "NDK_ERR_WIFI_INVDATA"	Invalid parameter
 *@li	\ref NDK_ERR_WIFI_DEVICE_NOTOPEN "NDK_ERR_WIFI_DEVICE_NOTOPEN"	SSID not found
 *@li	\ref NDK_ERR_WIFI_DEVICE_TIMEOUT "NDK_ERR_WIFI_DEVICE_TIMEOUT"	Timeout error
*/
int NDK_WiFiGetSec(const char *pszESSIDName, EM_WIFI_NET_SEC *pemSec);

/**
 *@brief	Connect to SSID
 *@param[in] pszESSIDName	SSID name
 *@param[in] pstParam	Connection parameters
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_WIFI_INVDATA "NDK_ERR_WIFI_INVDATA"	Invalid parameter
 *@li	\ref NDK_ERR_WIFI_DEVICE_BUSY "NDK_ERR_WIFI_DEVICE_BUSY"	Device busy
 *@li	\ref NDK_ERR_WIFI_DEVICE_UNAVAILABLE "NDK_ERR_WIFI_DEVICE_UNAVAILABLE"	Device unavailable
 *@li	\ref NDK_ERR_WIFI_UNKNOWN_ERROR "NDK_ERR_WIFI_UNKNOWN_ERROR"	Unknown error
*/
int NDK_WiFiConnect(const char *pszESSIDName, const ST_WIFI_PARAM *pstParam);

/**
 *@brief	Get connection status
 *@param[out]	pemState	Connection status
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_WIFI_INVDATA "NDK_ERR_WIFI_INVDATA"	Invalid parameter
 *@li	\ref NDK_ERR_WIFI_PROCESS_INBADSTATE "NDK_ERR_WIFI_PROCESS_INBADSTATE"	SSID not connected
*/
int NDK_WiFiConnectState(EM_WPA_CONSTATE *pemState);

/**
 *@brief	Check if SSID is connected
 *@return
 *@li	\ref NDK_OK "NDK_OK"  					Success
 *@li	\ref NDK_ERR_WIFI_DEVICE_UNAVAILABLE "NDK_ERR_WIFI_DEVICE_UNAVAILABLE"	Device unavailable
*/
int NDK_WiFiIsConnected(void);

/**
 *@brief Disconect SSID.
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR "NDK_ERR"				Fail
 *@li	\ref NDK_ERR_WIFI_DEVICE_UNAVAILABLE	Device unavailable
*/
int NDK_WiFiDisconnect(void);

/**
 *@brief	Shutdown WiFi
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref EM_NDK_ERR "EM_NDK_ERR"		Fail
*/
int NDK_WiFiShutdown(void);

/**
 *@brief	Get all scanned SSID info
 *@param[out]	pstList	List of SSID info
 *@param[in] unMaxNum	Maximum number of SSID
 *@param[out]	pnNumList   Number of SSID scanned
 *@return
  *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_WIFI_INVDATA "NDK_ERR_WIFI_INVDATA"	Invalid parameter
 *@li	\ref NDK_ERR_WIFI_DEVICE_FAULT "NDK_ERR_WIFI_DEVICE_FAULT"	Device fault
*/
int NDK_WiFiGetNetInfo(ST_WIFI_APINFO *pstList, unsigned int unMaxNum, int *pnNumList);

/**
 *@brief	Get MAC address
 *@param[out]	pszMac MAC address
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_WIFI_INVDATA "NDK_ERR_WIFI_INVDATA"	Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR"	 Fail
*/
int NDK_WiFiGetMac(char *pszMac);

/**
 *@brief	Enter sleep mode
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR "NDK_ERR"	 Fail
*/
int NDK_WiFiSleep(void);

/**
 *@brief	Wake up from sleep mode
 *@return
 *@li \ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR "NDK_ERR"	 Fail
*/
int NDK_WiFiWakeUp(void);

/** @}*/ // End of WiFi

/** @addtogroup  BarcodeScanner
* @{
*/

/**
 *@brief Scan types
*/
typedef enum {
        SCAN_SETTYPE_FLOODLIGHT     = 0, /**<Flood light*/
        SCAN_SETTYPE_FOCUSLIGHT     = 1, /**<Focus light*/
        SCAN_SETTYPE_SENSITIVITY    = 2, /**<Sensitivity*/
        SCAN_SETTYPE_FACTORYDEFAULT = 3  /**<Default*/
} EM_SCAN_SETTYPE;

/**
 *@brief	Initialize scanner
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				Success
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV"	 Failed to open device
 *@li	\ref NDK_ERR "NDK_ERR"	 Fail
*/
int NDK_ScanInit(void);

/**
 *@brief	Set scan parameter
 *@param[in]       emScanSet	Type (\ref EM_SCAN_SETTYPE "EM_SCAN_SETTYPE")
 *@param[in]       unSetValue
 		Light: (0: Flash; 1: No light; 2: Always on)\n
   		Focus: (0: Flicker; 1: No focus; 2: Induction)\n
		Sensitivity: 1-20 (The lower value, the higher sensitivity.)\n
                 Default settings: (Light: Flash; Focus: Flicker; Sensitivity: 11)
 *@return
 *@li	\ref NDK_OK "NDK_OK"  					Success
 *@li	\ref NDK_ERR "NDK_ERR"       			  Fail
 *@li \ref NDK_ERR_PARA "NDK_ERR_PARA"    		Invalid parameter
 *@li \ref NDK_ERR_NOT_SUPPORT "NDK_ERR_NOT_SUPPORT"	 Not supported
*/
int NDK_ScanSet(EM_SCAN_SETTYPE emScanSet, uint unSetValue);

/**
 *@brief	 Scan
 *@details Cancel key not supported
 *@param[in]      nTimeOut       Timeout in seconds
 *@param[out]	    pszValue        Scan data
 *@param[out]     pnLen           Scan data length
 *@return
 *@li	\ref NDK_OK "NDK_OK"  					  Success
 *@li	\ref NDK_ERR_TIMEOUT "NDK_ERR_TIMEOUT"      Timeout error
 *@li 	\ref NDK_ERR_PARA "NDK_ERR_PARA"    		 Invalid parameter
 *@li 	\ref NDK_ERR_OVERFLOW "NDK_ERR_OVERFLOW"      Buffer overflow
*/
int NDK_ScanDoScan(int nTimeOut, char *pszValue, int *pnLen);

/** @}*/ // End of barcode scanner

/** @addtogroup  Audio
* @{
*/

/**
 *@brief Voice control
*/
typedef enum {
        VOICE_CTRL_START  = 0, /**<Replay*/
        VOICE_CTRL_RESUME = 1, /**<Resume*/
        VOICE_CTRL_PAUSE  = 2, /**<Pause*/
        VOICE_CTRL_STOP   = 3, /**<Stop*/
        VOICE_CTRL_MUTE   = 4, /**<Mute*/
        VOICE_CTRL_NMUTE  = 5, /**<Not mute*/
        VOICE_CTRL_VOLUME = 6, /**<Volume*/
} EM_VOICE_CTRL;

/**
 *@brief	   Load audio file (Depreciated, will be removed in future release)
 *@param[in]       unVoiceId	 Audio deivce ID
 *@param[in]       pszFile	     Audio file name
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				 Success
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV"	 Failed to open device
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR"       		Fail
*/

int NDK_VoiceLoadFile(uint unVoiceId, char *pszFile);

/**
 *@brief	    Play back (Depreciated, will be removed in future release)
 *@param[in]       unVoiceId	Audio device ID
 *@param[in]       emCtrlId	Voice control options
 *@param[in]       unValue	   Volume: 0-4
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				  Success
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV"	Failed to open device
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	   Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR"       		 Fail
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL"	 Failed to call driver function
*/
int NDK_VoiceCtrl(uint unVoiceId, EM_VOICE_CTRL emCtrlId, uint unValue);

/**
 *@brief	Play wave file
 *@param[in]    pszName	Wave file path
 *@return
 *@li	\ref NDK_OK "NDK_OK"  				  Success
 *@li	\ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV"	Failed to open device
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"	   Invalid parameter
 *@li	\ref NDK_ERR "NDK_ERR"       		 Fail
 *@li	\ref NDK_ERR_IOCTL "NDK_ERR_IOCTL"	 Failed to call driver function
 *@li   \ref NDK_ERR_DEV_BUSY "NDK_ERR_DEV_BUSY"  Device busy
*/
int NDK_PlayWavFile(const char *pszName);

/** @}*/ // End of audio

/** @addtogroup SystemCall
* @{
*/

/**
 *@brief System call maps
*/
#define NDK_Ftok(pathname,proj_id) 	ftok(pathname,proj_id)
#define NDK_Shmget(key,size,shmflg)  shmget(key,size,shmflg)
#define NDK_Shmat(shmid,shmaddr,shmflg)   shmat(shmid,shmaddr,shmflg)
#define NDK_Fcntl(fd,cmd,arg)	fcntl(fd,cmd,arg)
#define NDK_Dlopen(pathname,mode)	dlopen(pathname,mode)
#define NDK_Dlclose(handle)		dlclose(handle)
#define NDK_Dlsym(handle,symbol)	dlsym(handle,symbol)
#define NDK_Kill(pid,sig)	kill(pid,sig)
#define NDK_Pause()		pause()
#define NDK_SetSockOpt(sockfd,level,optname,optval,optlen)	setsockopt(sockfd,level,optname,optval,optlen)
#define NDK_System(command)	system(command)
#define NDK_GetTimeOfDay(tv,tz)	gettimeofday(tv,tz)
#define NDK_FD_ZERO(set)	FD_ZERO(set)
#define NDK_FD_SET(fd,set)	FD_SET(fd,set)
#define NDK_Select(nfds,readfds,writefds,exceptfds,timeout)	select(nfds,readfds,writefds,exceptfds,timeout)
#define NDK_InetAddr(cp)	inet_addr(cp)
#define NDK_PthreadCreate(thread,attr,start_routine,arg) pthread_create(thread,attr,start_routine,arg)
#define NDK_PthreadJoin(thread,retval)  pthread_join(thread,retval)
#define NDK_PthreadExit(retval)  pthread_exit(retval)
#define NDK_FsGetcwd(buf,size)	getcwd(buf,size)
#define NDK_FsOpenDir(name)	opendir(name)
#define NDK_FsReadDir(dirp)	readdir(dirp)
#define NDK_FsCloseDir(dirp)	closedir(dirp)
#define NDK_FsFtruncate(fd,length)	ftruncate(fd,length)

/** @}*/ // End of system call

#endif

/* End of this file*/

