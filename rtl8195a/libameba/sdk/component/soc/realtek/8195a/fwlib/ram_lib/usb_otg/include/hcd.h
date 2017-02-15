#ifndef _HCD_H_
#define _HCD_H_


struct hc_driver {
        const char      *description;   /* "ehci-hcd" etc */
const char      *product_desc;  /* product/vendor string */
size_t          hcd_priv_size;  /* size of private data */

/* irq handler */
//irqreturn_t     (*irq) (struct usb_hcd *hcd);

int     flags;
#define HCD_MEMORY      0x0001          /* HC regs use memory (else I/O) */
#define HCD_LOCAL_MEM   0x0002          /* HC needs local memory */
#define HCD_SHARED      0x0004          /* Two (or more) usb_hcds share HW */
#define HCD_USB11       0x0010          /* USB 1.1 */
#define HCD_USB2        0x0020          /* USB 2.0 */
#define HCD_USB3        0x0040          /* USB 3.0 */
#define HCD_MASK        0x0070

        /* called to init HCD and root hub */
        int     (*reset) (struct usb_hcd *hcd);
        int     (*start) (struct usb_hcd *hcd);

        /* NOTE:  these suspend/resume calls relate to the HC as
         * a whole, not just the root hub; they're for PCI bus glue.
         */
        /* called after suspending the hub, before entering D3 etc */
//        int     (*pci_suspend)(struct usb_hcd *hcd, bool do_wakeup);

        /* called after entering D0 (etc), before resuming the hub */
//        int     (*pci_resume)(struct usb_hcd *hcd, bool hibernated);

        /* cleanly make HCD stop writing memory and doing I/O */
        void    (*stop) (struct usb_hcd *hcd);

        /* shutdown HCD */
  //      void    (*shutdown) (struct usb_hcd *hcd);

        /* return current frame number */
        int     (*get_frame_number) (struct usb_hcd *hcd);

        /* manage i/o requests, device state */
        int     (*urb_enqueue)(struct usb_hcd *hcd,
                                struct urb *urb);//, gfp_t mem_flags);
        int     (*urb_dequeue)(struct usb_hcd *hcd,
                                struct urb *urb, int status);

        /*
         * (optional) these hooks allow an HCD to override the default DMA
         * mapping and unmapping routines.  In general, they shouldn't be
         * necessary unless the host controller has special DMA requirements,
         * such as alignment contraints.  If these are not specified, the
         * general usb_hcd_(un)?map_urb_for_dma functions will be used instead
         * (and it may be a good idea to call these functions in your HCD
         * implementation)
         */
         #if 0
        int     (*map_urb_for_dma)(struct usb_hcd *hcd, struct urb *urb,
                                   gfp_t mem_flags);
        void    (*unmap_urb_for_dma)(struct usb_hcd *hcd, struct urb *urb);
        #endif
        /* hw synch, freeing endpoint resources that urb_dequeue can't */
        void    (*endpoint_disable)(struct usb_hcd *hcd,
                        struct usb_host_endpoint *ep);

        /* (optional) reset any endpoint state such as sequence number
           and current window */
        void    (*endpoint_reset)(struct usb_hcd *hcd,
                        struct usb_host_endpoint *ep);

        /* root hub support */
        int     (*hub_status_data) (struct usb_hcd *hcd, char *buf);
        int     (*hub_control) (struct usb_hcd *hcd,
                                u16 typeReq, u16 wValue, u16 wIndex,
                                char *buf, u16 wLength);
        #if 0                        
        int     (*bus_suspend)(struct usb_hcd *);
        int     (*bus_resume)(struct usb_hcd *);
        int     (*start_port_reset)(struct usb_hcd *, unsigned port_num);

                /* force handover of high-speed port to full-speed companion */
        void    (*relinquish_port)(struct usb_hcd *, int);
                /* has a port been handed over to a companion? */
        int     (*port_handed_over)(struct usb_hcd *, int);

                /* CLEAR_TT_BUFFER completion callback */
        void    (*clear_tt_buffer_complete)(struct usb_hcd *,
                                struct usb_host_endpoint *);

        /* xHCI specific functions */
                /* Called by usb_alloc_dev to alloc HC device structures */
        int     (*alloc_dev)(struct usb_hcd *, struct usb_device *);
                /* Called by usb_disconnect to free HC device structures */
        void    (*free_dev)(struct usb_hcd *, struct usb_device *);
        /* Change a group of bulk endpoints to support multiple stream IDs */
        int     (*alloc_streams)(struct usb_hcd *hcd, struct usb_device *udev,
                struct usb_host_endpoint **eps, unsigned int num_eps,
                unsigned int num_streams, gfp_t mem_flags);
        /* Reverts a group of bulk endpoints back to not using stream IDs.
         * Can fail if we run out of memory.
         */
        int     (*free_streams)(struct usb_hcd *hcd, struct usb_device *udev,
                struct usb_host_endpoint **eps, unsigned int num_eps,
                gfp_t mem_flags);

        /* Bandwidth computation functions */
        /* Note that add_endpoint() can only be called once per endpoint before
         * check_bandwidth() or reset_bandwidth() must be called.
         * drop_endpoint() can only be called once per endpoint also.
         * A call to xhci_drop_endpoint() followed by a call to
         * xhci_add_endpoint() will add the endpoint to the schedule with
         * possibly new parameters denoted by a different endpoint descriptor
         * in usb_host_endpoint.  A call to xhci_add_endpoint() followed by a
         * call to xhci_drop_endpoint() is not allowed.
         */
                /* Allocate endpoint resources and add them to a new schedule */
        int     (*add_endpoint)(struct usb_hcd *, struct usb_device *,
                                struct usb_host_endpoint *);
                /* Drop an endpoint from a new schedule */
        int     (*drop_endpoint)(struct usb_hcd *, struct usb_device *,
                                 struct usb_host_endpoint *);
                /* Check that a new hardware configuration, set using
                 * endpoint_enable and endpoint_disable, does not exceed bus
                 * bandwidth.  This must be called before any set configuration
                 * or set interface requests are sent to the device.
                 */
        int     (*check_bandwidth)(struct usb_hcd *, struct usb_device *);
                /* Reset the device schedule to the last known good schedule,
                 * which was set from a previous successful call to
                 * check_bandwidth().  This reverts any add_endpoint() and
                 * drop_endpoint() calls since that last successful call.
                 * Used for when a check_bandwidth() call fails due to resource
                 * or bandwidth constraints.
                 */
        void    (*reset_bandwidth)(struct usb_hcd *, struct usb_device *);
                /* Returns the hardware-chosen device address */
        int     (*address_device)(struct usb_hcd *, struct usb_device *udev);
                /* Notifies the HCD after a hub descriptor is fetched.
                 * Will block.
                 */
        int     (*update_hub_device)(struct usb_hcd *, struct usb_device *hdev,
                        struct usb_tt *tt, gfp_t mem_flags);
        int     (*reset_device)(struct usb_hcd *, struct usb_device *);
                /* Notifies the HCD after a device is connected and its
                 * address is set
                 */
        int     (*update_device)(struct usb_hcd *, struct usb_device *);
        int     (*set_usb2_hw_lpm)(struct usb_hcd *, struct usb_device *, int);
        /* USB 3.0 Link Power Management */
                /* Returns the USB3 hub-encoded value for the U1/U2 timeout. */
        int     (*enable_usb3_lpm_timeout)(struct usb_hcd *,
                        struct usb_device *, enum usb3_link_state state);
                /* The xHCI host controller can still fail the command to
                 * disable the LPM timeouts, so this can return an error code.
                 */
        int     (*disable_usb3_lpm_timeout)(struct usb_hcd *,
                        struct usb_device *, enum usb3_link_state state);
        int     (*find_raw_port_number)(struct usb_hcd *, int);
        #endif
};

#endif
