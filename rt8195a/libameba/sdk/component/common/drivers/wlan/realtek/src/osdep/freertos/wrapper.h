#ifndef __WRAPPER_H__
#define __WRAPPER_H__
/**************************************************************************
  *  Wrapper provide a linux-like interface 
  *
  *  Copyright (c) 2013 Realtek Semiconductor Corp.
  ************************************************************************/

//----- ------------------------------------------------------------------
// Include Files
//----- ------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "wireless.h"
#include <skbuff.h>
#include "freertos_service.h"

#ifndef __LIST_H
#warning "DLIST_NOT_DEFINE!!!!!!"
//----- ------------------------------------------------------------------
// Linled List
//----- ------------------------------------------------------------------
/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */
// struct list_head {
// 	struct list_head *next, *prev;
// };

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

/*
 * Insert a new entry between two known consecutive entries. 
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static __inline void __list_add(struct list_head * new,
	struct list_head * prev,
	struct list_head * next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static __inline void __list_del(struct list_head * prev,
				  struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is in an undefined state.
 */
static __inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static __inline void list_del_init(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry); 
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static __inline int list_empty(struct list_head *head)
{
	return head->next == head;
}

/**
 * list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static __inline void list_splice(struct list_head *list, struct list_head *head)
{
	struct list_head *first = list->next;

	if (first != list) {
		struct list_head *last = list->prev;
		struct list_head *at = head->next;

		first->prev = head;
		head->next = first;

		last->next = at;
		at->prev = last;
	}
}

void list_add(struct list_head *new, struct list_head *head);
void list_add_tail(struct list_head *new, struct list_head *head);
#endif

extern void save_and_cli(void);
extern void restore_flags(void);
//----- ------------------------------------------------------------------
// SKB Operation
//----- ------------------------------------------------------------------

#define SMP_CACHE_BYTES		4
#define SKB_DATA_ALIGN(X)	(((X) + (SMP_CACHE_BYTES - 1)) & ~(SMP_CACHE_BYTES - 1))

// Consideration for SKB size
//	Tx: [INTF_CMD][TX_DESC][WLAN_HDR][QoS][IV][SNAP][Data][MIC][ICV][INTF_STATUS]
//			Since SKB is used to accept ethernet packet from upper layer, SKB length of WLAN_MAX_ETHFRM_LEN 
//			(= 1514) is enough. But since SKB is also used to get spi receive packet, overall buffer space 
//			should be taken into consideration.
//	RX: [INTF_CMD][RX_DESC][Drv_Info][WLAN_HDR][QoS][IV][SNAP][Data][MIC][ICV][CRC][INTF_STATUS]
//
//	32: Driver_Info that carry phy related information for each packets. Required only for receive case.
//	WLAN_MAX_ETHFRM_LEN : May not be required because WLAN_HEADER +SNAP can totally 
//			cover ethernet header. Keep in only for safety.
//
//     **Notes** SDIO requires 512 blocks r/w, so 512*4 = 2048 is required.
//	2003/12/26. The value is reduced from 2048 to 1658 for GSPI
//	2014/02/05. The value is 1650 for 8195A LX_BUS
#define SKB_RESERVED_FOR_SAFETY	0
#define SKB_WLAN_TX_EXTRA_LEN	(TXDESC_SIZE + WLAN_HDR_A4_QOS_LEN + WLAN_MAX_IV_LEN + WLAN_SNAP_HEADER - WLAN_ETHHDR_LEN)
#define RX_DRIVER_INFO				32

#if (defined CONFIG_GSPI_HCI || defined CONFIG_SDIO_HCI)
#define HAL_INTERFACE_OVERHEAD_SKB_DATA 12	//HAL_INTERFACE_CMD (4) + HAL_INTERFACE_STATUS (8)
#elif defined(CONFIG_LX_HCI)
#define HAL_INTERFACE_OVERHEAD_SKB_DATA 0
#endif

#if defined CONFIG_GSPI_HCI || defined CONFIG_SDIO_HCI || defined(CONFIG_LX_HCI)
	#if defined(CONFIG_RTL8195A) || defined(CONFIG_RTL8711B)
		#if defined(CONFIG_MP_INCLUDED)
			#ifdef CONFIG_DONT_CARE_TP
				#define MAX_RX_PKT_LIMIT		((WLAN_MAX_PROTOCOL_OVERHEAD + WLAN_MAX_RX_ETHFRM_LEN + 511) / 512) // 4, for lxbus
			#else
				#define MAX_RX_PKT_LIMIT		((WLAN_MAX_PROTOCOL_OVERHEAD + WLAN_MAX_ETHFRM_LEN + 511) / 512) // 4, for lxbus
			#endif
			#define MAX_RX_PKT_SIZE				MAX_RX_PKT_LIMIT*512	// MAX_SKB_BUF_SIZE = 0+32+40+512*4+0 = 2120
		#else
			#ifdef CONFIG_DONT_CARE_TP
				#define MAX_RX_PKT_SIZE			WLAN_MAX_PROTOCOL_OVERHEAD + WLAN_MAX_RX_ETHFRM_LEN
			#else
				#define MAX_RX_PKT_SIZE			WLAN_MAX_PROTOCOL_OVERHEAD + WLAN_MAX_ETHFRM_LEN
			#endif
			#define MAX_RX_PKT_LIMIT			((MAX_RX_PKT_SIZE + 511) / 512)			// MAX_RX_PKT_SIZE = 64+1514 = 1578
		#endif
	#else
		#ifdef CONFIG_DONT_CARE_TP
			#define MAX_RX_PKT_SIZE				WLAN_MAX_PROTOCOL_OVERHEAD + WLAN_MAX_RX_ETHFRM_LEN
		#else
			#define MAX_RX_PKT_SIZE				WLAN_MAX_PROTOCOL_OVERHEAD + WLAN_MAX_ETHFRM_LEN
		#endif
	#endif
	
	#ifdef CONFIG_DONT_CARE_TP
		#define MAX_TX_SKB_BUF_SIZE				(HAL_INTERFACE_OVERHEAD_SKB_DATA+RX_DRIVER_INFO+\
												((TXDESC_SIZE>RXDESC_SIZE)? TXDESC_SIZE:RXDESC_SIZE) +\
												WLAN_MAX_PROTOCOL_OVERHEAD + WLAN_MAX_TX_ETHFRM_LEN +\
												SKB_RESERVED_FOR_SAFETY)
		#define MAX_RX_SKB_BUF_SIZE				(HAL_INTERFACE_OVERHEAD_SKB_DATA+RX_DRIVER_INFO+\
												((TXDESC_SIZE>RXDESC_SIZE)? TXDESC_SIZE:RXDESC_SIZE) +\
												MAX_RX_PKT_SIZE +\
												SKB_RESERVED_FOR_SAFETY)
	#else
		#define MAX_SKB_BUF_SIZE				(HAL_INTERFACE_OVERHEAD_SKB_DATA+RX_DRIVER_INFO+\
												((TXDESC_SIZE>RXDESC_SIZE)? TXDESC_SIZE:RXDESC_SIZE) +\
												MAX_RX_PKT_SIZE +\
												SKB_RESERVED_FOR_SAFETY)
	#endif
#else
#define MAX_SKB_BUF_SIZE	2048
#endif

#if 0
struct  sk_buff_head {
	struct list_head	*next, *prev;
	u32			qlen;
};

struct sk_buff {
	/* These two members must be first. */
	struct sk_buff		*next;		/* Next buffer in list */
	struct sk_buff		*prev;		/* Previous buffer in list */
	
	struct sk_buff_head	*list;		/* List we are on */	
	unsigned char		*head;		/* Head of buffer */
	unsigned char		*data;		/* Data head pointer */
	unsigned char		*tail;		/* Tail pointer	*/
	unsigned char		*end;		/* End pointer */ 
	struct net_device 	*dev;		/* Device we arrived on/are leaving by */	
	unsigned int 		len;		/* Length of actual data */	
};

/**
 *	skb_put - add data to a buffer
 *	@skb: buffer to use 
 *	@len: amount of data to add
 *
 *	This function extends the used data area of the buffer. If this would
 *	exceed the total buffer size the kernel will panic. A pointer to the
 *	first byte of the extra data is returned.
 */
 
static __inline__ unsigned char *skb_put(struct sk_buff *skb, unsigned int len)
{
	unsigned char *tmp=skb->tail;
	skb->tail+=len;
	skb->len+=len;
	if(skb->tail>skb->end) {
		ASSERT(0);		
	}
	
	return tmp;
}

static __inline__ unsigned char *__skb_pull(struct sk_buff *skb, unsigned int len)
{
	skb->len-=len;
	skb->data = (unsigned char *)(((unsigned int)skb->data) + len);

	return skb->data;
}

/**
 *	skb_reserve - adjust headroom
 *	@skb: buffer to alter
 *	@len: bytes to move
 *
 *	Increase the headroom of an empty &sk_buff by reducing the tail
 *	room. This is only allowed for an empty buffer.
 */

static __inline__ void skb_reserve(struct sk_buff *skb, unsigned int len)
{
	skb->data+=len;
	skb->tail+=len;
}

static __inline__ void skb_queue_head_init(struct sk_buff_head *list)
{
        list->prev = (struct list_head *)list;
        list->next = (struct list_head *)list;
        list->qlen = 0;
}

/**
 *      __skb_queue_tail - queue a buffer at the list tail
 *      @list: list to use
 *      @newsk: buffer to queue
 *
 *      Queue a buffer at the end of a list. This function takes no locks
 *      and you must therefore hold required locks before calling it.
 *
 *      A buffer cannot be placed on two lists at the same time.
 */     

static __inline__ void __skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk)
{
        struct sk_buff *prev, *next;

        newsk->list = list;
        list->qlen++;
        next = (struct sk_buff *)list;
        prev = next->prev;
        newsk->next = next;
        newsk->prev = prev;
        next->prev = newsk;
        prev->next = newsk;
}

/**
 *      skb_queue_tail - queue a buffer at the list tail
 *      @list: list to use
 *      @newsk: buffer to queue
 *
 *      Queue a buffer at the tail of the list. This function takes the
 *      list lock and can be used safely with other locking &sk_buff functions
 *      safely.
 *
 *      A buffer cannot be placed on two lists at the same time.
 */     

static __inline__ void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk)
{
        save_and_cli();
        __skb_queue_tail(list, newsk);
        restore_flags();
}

static __inline__ void skb_assign_buf(struct sk_buff *skb, unsigned char *buf, unsigned int len)
{	
	skb->head = buf;
	skb->data = buf;
	skb->tail = buf;
	skb->end = buf + len;
}

static __inline__ unsigned char *skb_tail_pointer(const struct sk_buff *skb)
{
	return skb->tail;
}

static __inline__ void skb_reset_tail_pointer(struct sk_buff *skb)
{
	skb->tail = skb->data;
}

static __inline__ void skb_set_tail_pointer(struct sk_buff *skb, const int offset)
{
	skb->tail = skb->data + offset;
}

static __inline__ unsigned char *skb_end_pointer(const struct sk_buff *skb)
{
	return skb->end;
}
#endif
/*
 *	External functions
 */
struct net_device;
extern void kfree_skb_chk_key(struct sk_buff *skb, struct net_device *root_dev);
#ifdef CONFIG_TRACE_SKB
extern void show_skb(void);
extern int _set_skb_list_flag(struct sk_buff *skb, unsigned int queueflag);
extern void dump_skb_list(void);
#define set_skb_list_flag(skb, queueflag) \
	(\
		_set_skb_list_flag((skb), queueflag), \
		(skb) ? (skb)->funcname[(skb)->list_idx] = __FUNCTION__:NULL \
	)
extern int _clear_skb_list_flag(struct sk_buff *skb, unsigned int queueflag);
#define clear_skb_list_flag(skb, queueflag) \
	(\
		_clear_skb_list_flag((skb), queueflag), \
		(skb) ? (skb)->funcname[(skb)->list_idx] = __FUNCTION__ : NULL \
	)
#define dev_kfree_skb_any(trx, holder, skb)	\
	do{\
		clear_skb_list_flag(skb, SKBLIST_##trx##holder##_MASK);\
		set_skb_list_flag(skb, SKBLIST_POOL);\
		kfree_skb_chk_key(skb, skb->dev);\
	}while (0)
#else
#define dev_kfree_skb_any(skb)	kfree_skb_chk_key(skb, skb->dev)
#endif
extern struct sk_buff *dev_alloc_skb(unsigned int length, unsigned int reserve_len);
extern struct sk_buff *skb_clone(struct sk_buff *skb, int gfp_mask);
extern struct sk_buff *skb_copy(const struct sk_buff *skb, int gfp_mask, unsigned int reserve_len);
extern unsigned char *skb_pull(struct sk_buff *skb, unsigned int len);

//----- ------------------------------------------------------------------
// Device structure
//----- ------------------------------------------------------------------
struct net_device_stats {
	unsigned long   rx_packets;             /* total packets received       */
	unsigned long   tx_packets;             /* total packets transmitted    */
	unsigned long   rx_dropped;             /* no space in linux buffers    */
	unsigned long   tx_dropped;             /* no space available in linux  */
	unsigned long   rx_bytes;               /* total bytes received         */
	unsigned long   tx_bytes;               /* total bytes transmitted      */
	unsigned long   rx_overflow;            /* rx fifo overflow count       */
};

struct net_device {
	char			name[16];
	void			*priv;		/* pointer to private data */
	unsigned char		dev_addr[6];	/* set during bootup */
	int (*init)(void);
	int (*open)(struct net_device *dev);
	int (*stop)(struct net_device *dev);
	int (*hard_start_xmit)(struct sk_buff *skb, struct net_device *dev);
	int (*do_ioctl)(struct net_device *dev, struct iwreq *ifr, int cmd);
	struct net_device_stats* (*get_stats)(struct net_device *dev);
};

typedef struct {
	struct net_device *dev;		/* Binding wlan driver netdev */
	void *skb;			/* pending Rx packet */
	unsigned int tx_busy;
	unsigned int rx_busy;
	unsigned char enable;
	unsigned char mac[6];
} Rltk_wlan_t;

#define netdev_priv(dev)		dev->priv

extern struct net_device *alloc_etherdev(int sizeof_priv);
void free_netdev(struct net_device *dev);
int dev_alloc_name(struct net_device *net_dev, const char *ifname);


//----- ------------------------------------------------------------------
// Timer Operation
//----- ------------------------------------------------------------------
void init_timer(struct timer_list *timer);
void mod_timer(struct timer_list *timer, u32 delay_time_ms);
void  cancel_timer_ex(struct timer_list * timer);
void del_timer_sync(struct timer_list * timer);
void init_timer_wrapper(void);
void deinit_timer_wrapper(void);

#endif //__WRAPPER_H__



