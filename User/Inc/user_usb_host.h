#ifndef USER_USB_HOST_H
#define USER_USB_HOST_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENABLE_USB_HOST

extern bool user_usb_host_is_mount(void);
extern bool user_mount_Udisk(void);
extern bool user_unmount_Udisk(void);
extern void user_usb_host_update_status(void);

#endif //ENABLE_USB_HOST

#ifdef __cplusplus
} //extern "C"
#endif

#endif // USER_USB_HOST_H

