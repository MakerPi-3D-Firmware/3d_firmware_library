#include "user_common.h"

#ifdef ENABLE_USB_HOST

#define USER_USB_HOST_MOUNT 0
#define USER_USB_HOST_UNMOUNT 1

extern ApplicationTypeDef Appli_state;

static volatile ApplicationTypeDef Last_Appli_state = APPLICATION_IDLE;
static volatile bool is_usb_mount = false;
static volatile uint8_t usb_host_status = USER_USB_HOST_UNMOUNT;

bool user_usb_host_is_mount(void)
{
  return (usb_host_status == USER_USB_HOST_MOUNT);
}

bool user_mount_Udisk(void)
{
  if (is_usb_mount)
  {
    return true;
  }

  if (f_mount(&USBHFatFS, USBHPath, 0) == FR_OK)
  {
    USER_EchoLog("user_mount_Udisk ok!");
    is_usb_mount = true;
  }
  else
  {
    USER_EchoLog("user_mount_Udisk not ok!");
    return false;
  }

  return true;
}

bool user_unmount_Udisk(void)
{
  if (!is_usb_mount)
  {
    return true;
  }

  if (f_mount(NULL, USBHPath, 0) == FR_OK)
  {
    USER_EchoLog("user_unmount_Udisk ok!");
    is_usb_mount = false;
  }
  else
  {
    USER_EchoLog("user_unmount_Udisk not ok!");
    return false;
  }

  return true;
}

void user_usb_host_update_status(void)
{
  if (Appli_state == APPLICATION_START)
  {
#ifdef ENABLE_FREERTOS
    osDelay(20);
#endif
  }

  if ((Last_Appli_state == APPLICATION_DISCONNECT || Last_Appli_state == APPLICATION_IDLE) &&
      Appli_state == APPLICATION_READY && usb_host_status == USER_USB_HOST_UNMOUNT)
  {
    if (is_usb_mount)
    {
      return;
    }

    if (f_mount(&USBHFatFS, USBHPath, 1) == FR_OK)
    {
      Last_Appli_state = APPLICATION_READY;
      usb_host_status = USER_USB_HOST_MOUNT;
      is_usb_mount = true;
      USER_EchoLog("Udisk Insert!");
    }
  }

  if (Last_Appli_state == APPLICATION_READY && Appli_state == APPLICATION_DISCONNECT && usb_host_status == USER_USB_HOST_MOUNT)
  {
    if (!is_usb_mount)
    {
      return;
    }

    if (f_mount(NULL, USBHPath, 1) == FR_OK)
    {
      Last_Appli_state = APPLICATION_DISCONNECT;
      usb_host_status = USER_USB_HOST_UNMOUNT;
      is_usb_mount = false;
      USER_EchoLog("Udisk Remove!");
    }
  }
}

#endif // ENABLE_USB_HOST


