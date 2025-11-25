/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helios_os.h"
#include "stackctrl.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "callbackdeal.h"
#include "py/obj.h"
#if MICROPY_ENABLE_CALLBACK_DEAL


#ifdef QPY_CALLBACKDEAL_LOG_ENABLE
#if defined(PLAT_ASR) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_ASR_1602)
extern void ql_log_mask_set(unsigned long long module_mask, unsigned int port_mask);
extern void _ql_app_log(const char* fmt, ...);
static char usb_trace_init_flag = 0;
#define qpy_callbackdeal_trace(fmt, ...) do{                                                                            \
                                                if(!usb_trace_init_flag)                                                \
                                                {                                                                       \
                                                    ql_log_mask_set(0x20, 0x02);                                        \
                                                    usb_trace_init_flag = 1;                                            \
                                                }                                                                       \
                                                _ql_app_log("[%s][%s, L%d]"fmt"", "cbdl", __FUNCTION__, __LINE__, ##__VA_ARGS__);   \
                                           }while(0)

#elif defined(PLAT_Unisoc) || defined(PLAT_Unisoc_8850) || defined(PLAT_Unisoc_8910_R05) || defined(PLAT_Unisoc_8850_R02) || defined(PLAT_Unisoc_8910_R06)
#include "helios_debug.h"
#define qpy_callbackdeal_trace(fmt, ...) Helios_Debug_Output("[%s][%s, L%d]"fmt"", "cbdl", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#error "The platform not support usb trace!!!"
#endif
#else
#define qpy_callbackdeal_trace(fmt, ...)
#endif



#define QPY_CALLBACK_DEAL_MSG_MAX_NUM (2 * MICROPY_SCHEDULER_DEPTH)
#if defined(PLAT_ECR6600) || defined (PLAT_aic8800m40)
#define QPY_CALLBACK_DEAL_THREAD_STACK_SIZE (4 *1024)
#else
#define QPY_CALLBACK_DEAL_THREAD_STACK_SIZE (16 *1024)
#endif

static Helios_MsgQ_t qpy_callback_deal_queue = 0;
Helios_Thread_t qpy_callback_deal_task_ref = 0;

typedef struct _callback_deal_thread_entry_args_t {
    mp_obj_dict_t *dict_locals;
    mp_obj_dict_t *dict_globals;
} callback_deal_thread_entry_args_t;

#if MICROPY_QPY_MODULE_BT
extern void qpy_bt_msg_proc(void *msg);
#endif
#if MICROPY_QPY_MODULE_BLE
extern void qpy_ble_msg_proc(void *msg);
#endif

#if MICROPY_QPY_MODULE_POC
extern void qpy_poc_msg_proc ( void *msg );
#endif //end MICROPY_QPY_MODULE_POC

#if MICROPY_QPY_MODULE_LWM2M
extern void qpy_lwm2m_msg_proc(void *param);
#endif //end MICROPY_QPY_MODULE_LWM2M

#if MICROPY_QPY_MODULE_ESIM && MICROPY_QPY_MODULE_ESIM_IPA
extern void qpy_esim_msg_proc(void *param);
#endif
/*
 * Create a task&queue that handles callback tasks exclusively
 */
void qpy_callback_deal_thread(void * args_in);

/*Jayceon 2022/08/31  add for QuecPython  --start*/
extern bool mp_obj_is_boundmeth(mp_obj_t o);
extern mp_obj_t mp_obj_bound_get_self(void *bound);
extern mp_obj_t mp_obj_bound_get_meth(void *bound);
extern bool mp_obj_is_closure(void *obj);
extern mp_obj_t mp_obj_closure_get_fun(void *closure);

typedef struct temp_mp_obj_bound_meth_t {
    mp_obj_base_t base;
    mp_obj_t meth;
    mp_obj_t self;
} temp_mp_obj_bound_meth_t;

typedef struct temp_mp_obj_closure_t {
    mp_obj_base_t base;
    mp_obj_t fun;
    size_t n_closed;
    mp_obj_t closed[];
} temp_mp_obj_closure_t;

mp_obj_t mp_obj_bound_get_self(void *bound)
{
    if(NULL == bound)
        return NULL;

    return ((temp_mp_obj_bound_meth_t *)bound)->self;
}

mp_obj_t mp_obj_bound_get_meth(void *bound)
{
    if(NULL == bound)
        return NULL;

    return ((temp_mp_obj_bound_meth_t *)bound)->meth;
}

bool mp_obj_is_boundmeth(mp_obj_t o) {

    return mp_obj_is_type(o, &mp_type_bound_meth);
}

extern const mp_obj_type_t mp_type_closure;
bool mp_obj_is_closure(void *obj)
{
    return mp_obj_is_type(obj, &mp_type_closure);
}

mp_obj_t mp_obj_closure_get_fun(void *closure){
    if(NULL == closure)
        return NULL;

    return ((temp_mp_obj_closure_t *)closure)->fun;
}

extern void mp_main_thread_wakeup();
static bool _mp_sched_schedule_ex(c_callback_t *callback) {

	// When executing code within a handler we must lock the scheduler to
	// prevent any scheduled callbacks from running, and lock the GC to
	// prevent any memory allocations.
	mp_sched_lock();
	gc_lock();
	nlr_buf_t nlr;
	if (nlr_push(&nlr) == 0) {
		mp_sched_schedule(callback->cb, callback->arg);
		nlr_pop();
	} else {
		// Uncaught exception; disable the callback so that it doesn't run again
		mp_printf(&mp_plat_print, "Uncaught exception in IRQ callback handler\n");
		mp_obj_print_exception(MICROPY_ERROR_PRINTER, MP_OBJ_FROM_PTR(nlr.ret_val));
        gc_unlock();
	    mp_sched_unlock();
        return false;
	}
	gc_unlock();
	mp_sched_unlock();

    mp_main_thread_wakeup();
    return true;
}

//Add a callback to the unscheduled table. If it is a Method,
//load it first so that the recorded callback is not collected by the GC
bool mp_sched_schedule_ex(c_callback_t *callback, mp_obj_t arg)
{
    (void)arg;
    if(NULL == callback)
    {
        return false;
    }

    if((false == callback->is_method) && mp_obj_is_callable(callback->cb))//function
    {
        return _mp_sched_schedule_ex(callback);
    }
    else if(true == callback->is_method)//bound_method
    {
        if(mp_obj_is_boundmeth(callback->cb)) {
            return _mp_sched_schedule_ex(callback);
        } else {
            mp_obj_t cb = mp_load_attr(callback->method_self, callback->method_name);
            callback->cb = cb;//record a new boundmeth
            return _mp_sched_schedule_ex(callback);
        }
    }
    else
    {
	    return false;
	}
}

//Register callback
bool mp_sched_schedule_callback_register(c_callback_t *callback, mp_obj_t func_obj)
{
    if(NULL == callback || NULL == func_obj) {
        return false;
    }
    //memset(callback, 0 , sizeof(c_callback_t));
    if(mp_const_none != func_obj)
    {
        if(mp_obj_is_boundmeth(func_obj))//is bound_meth. Records the object for this method, along with the bytecode for the name of this method
        {
            callback->is_method = true;
            callback->method_self = mp_obj_bound_get_self(func_obj);
            mp_obj_t fun = mp_obj_bound_get_meth((int*)func_obj);
            if(mp_obj_is_closure(fun)) {
                callback->method_name = mp_obj_fun_get_name(mp_obj_closure_get_fun(fun));
            } else {
                callback->method_name = mp_obj_fun_get_name(fun);
            }
            callback->cb = func_obj;
            return true;
        }
    }

    callback->is_method = false;
    callback->cb = func_obj;

    return true;
}

void qpy_callback_deal_init(void)
{
#if !defined(PLAT_RDA)
	qpy_callback_deal_queue = Helios_MsgQ_Create(QPY_CALLBACK_DEAL_MSG_MAX_NUM, sizeof(st_CallBack_Deal));
#endif
	// create thread
	callback_deal_thread_entry_args_t *th_args;
	th_args = m_new_obj(callback_deal_thread_entry_args_t);
	// pass our locals and globals into the new thread
    th_args->dict_locals = mp_locals_get();
    th_args->dict_globals = mp_globals_get();
    
    Helios_ThreadAttr ThreadAttr = {
        .name = "cb_deal",
        .stack_size = QPY_CALLBACK_DEAL_THREAD_STACK_SIZE,
        .priority = (MP_THREAD_PRIORITY - 2),//high than mpthread
        .entry = (void*)qpy_callback_deal_thread,
        .argv = (void*)th_args
    };
    qpy_callback_deal_task_ref = Helios_Thread_Create(&ThreadAttr);
    //add thread to python_thread link list, the thread is automatically deleted after the VM restarts.
    mp_new_thread_add((uint32_t)qpy_callback_deal_task_ref, QPY_CALLBACK_DEAL_THREAD_STACK_SIZE);
}

/*
 * Returns whether the current thread is callbackdeal thread
 */
bool qpy_is_callback_deal_thread(void)
{
    return (Helios_Thread_GetID() == qpy_callback_deal_task_ref);
}

/*
 * delete callback_deal task&queue
 */
void qpy_callback_deal_deinit(void)
{
	Helios_MsgQ_Delete(qpy_callback_deal_queue); qpy_callback_deal_queue = (Helios_MsgQ_t)0;
	//Just set it to null and the thread will be deleted automatically when the vm restarts
	qpy_callback_deal_task_ref = (Helios_Thread_t)0;
}


//Added by Freddy @20211124 发送消息至deal task的queue
int qpy_send_msg_to_callback_deal_thread(uint8_t id, void *msg)
{
	int ret = -1;
    if((Helios_MsgQ_t)0 != qpy_callback_deal_queue)
    {
        st_CallBack_Deal cb_msg = {0};
        cb_msg.callback_type_id = id;
        cb_msg.msg = msg;
        qpy_callbackdeal_trace("callback_type_id:%d", id);
        ret = Helios_MsgQ_Put(qpy_callback_deal_queue, (void*)(&cb_msg), sizeof(st_CallBack_Deal), HELIOS_NO_WAIT);
    }
	return ret;
}


st_CallBack_LoadBoundMeth * qpy_callback_para_node_add(c_callback_t *callback, mp_obj_t arg)
{
    st_CallBack_LoadBoundMeth *loadboundmet_msg = calloc(1, sizeof(st_CallBack_LoadBoundMeth));
    if(NULL == loadboundmet_msg)
    {
        return NULL;
    }
#if defined(PLAT_ASR) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_ASR_1602)
    int rc = Helios_Critical_Enter();
#endif
    loadboundmet_msg->next = (st_CallBack_LoadBoundMeth *)MP_STATE_PORT(loadmeth_para);
    loadboundmet_msg->callback = callback;
    loadboundmet_msg->arg = arg;
    MP_STATE_PORT(loadmeth_para) = (void *)loadboundmet_msg;
    qpy_callbackdeal_trace("loadmeth_para:0x%x", MP_STATE_PORT(loadmeth_para));
#if defined(PLAT_ASR) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_ASR_1602)
    Helios_Critical_Exit(rc);
#endif

    return loadboundmet_msg;
}


void qpy_callback_para_node_delete(st_CallBack_LoadBoundMeth *cb_msg)
{
#if defined(PLAT_ASR) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_ASR_1602)
    int rc = Helios_Critical_Enter();
#endif
    qpy_callbackdeal_trace("loadmeth_para:0x%x", MP_STATE_PORT(loadmeth_para));
    st_CallBack_LoadBoundMeth *loadmeth_para_head = (st_CallBack_LoadBoundMeth *)MP_STATE_PORT(loadmeth_para);
    st_CallBack_LoadBoundMeth *loadmeth_para_tmp = loadmeth_para_head;
    for(;loadmeth_para_head != NULL; loadmeth_para_head = loadmeth_para_head->next)
    {
        if(loadmeth_para_head == cb_msg)
        {
            qpy_callbackdeal_trace("cb_msg:0x%x", cb_msg);
            if(loadmeth_para_head == (st_CallBack_LoadBoundMeth *)MP_STATE_PORT(loadmeth_para))
            {
                MP_STATE_PORT(loadmeth_para) = loadmeth_para_head->next;
                qpy_callbackdeal_trace("loadmeth_para:0x%x", MP_STATE_PORT(loadmeth_para));
            }
            else
            {
                loadmeth_para_tmp->next = loadmeth_para_head->next;
            }
            break;
        }
        loadmeth_para_tmp = loadmeth_para_head;
    }
#if defined(PLAT_ASR) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_ASR_1602)
    Helios_Critical_Exit(rc);
#endif

}

void qpy_callback_para_link_mark_in_used(void)
{

#if defined(PLAT_ASR) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_ASR_1602)
   int rc = Helios_Critical_Enter();
#endif
   st_CallBack_LoadBoundMeth *loadmeth_para_head = (st_CallBack_LoadBoundMeth *)MP_STATE_PORT(loadmeth_para);
   for(;loadmeth_para_head != NULL; loadmeth_para_head = loadmeth_para_head->next)
   {
       if(NULL != loadmeth_para_head->arg)
       {
           gc_collect_root((void **)&loadmeth_para_head->arg, 1);
       }
   }
#if defined(PLAT_ASR) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_ASR_1602)
   Helios_Critical_Exit(rc);
#endif

}


void qpy_callback_para_link_free_all(void)
{
#if defined(PLAT_ASR) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_ASR_1602)
    int rc = Helios_Critical_Enter();
#endif
    st_CallBack_LoadBoundMeth *loadmeth_para_head = (st_CallBack_LoadBoundMeth *)MP_STATE_PORT(loadmeth_para);
    st_CallBack_LoadBoundMeth *loadmeth_para_tmp = NULL;
    while(loadmeth_para_head != NULL)
    {
        loadmeth_para_tmp = loadmeth_para_head;
        loadmeth_para_head = loadmeth_para_head->next;
        free(loadmeth_para_tmp);
    }
    MP_STATE_PORT(loadmeth_para) = NULL;
#if defined(PLAT_ASR) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_ASR_1602)
    Helios_Critical_Exit(rc);
#endif

}


bool mp_sched_empty(void);
/*
 * Resolve the crash issue when non-Python threads apply for GC memory and throw exceptions when memory application fails
 */
void qpy_callback_deal_thread(void * args_in)
{
#if !defined(PLAT_RDA)
	qpy_callback_deal_queue = Helios_MsgQ_Create(QPY_CALLBACK_DEAL_MSG_MAX_NUM, sizeof(st_CallBack_Deal));
#endif
	callback_deal_thread_entry_args_t *args = (callback_deal_thread_entry_args_t *)args_in;
    mp_state_thread_t ts;
    mp_thread_set_state(&ts);
    st_CallBack_Deal msg = {0};

    mp_stack_set_top(&ts + 1); // need to include ts in root-pointer scan
#if defined(PLAT_ECR6600) || defined (PLAT_aic8800m40)
    mp_stack_set_limit(QPY_CALLBACK_DEAL_THREAD_STACK_SIZE * sizeof(uint32_t) - 1024);
#else
    mp_stack_set_limit(QPY_CALLBACK_DEAL_THREAD_STACK_SIZE - 1024);
#endif

    #if MICROPY_ENABLE_PYSTACK
    // TODO threading and pystack is not fully supported, for now just make a small stack
    mp_obj_t mini_pystack[128];
    mp_pystack_init(mini_pystack, &mini_pystack[128]);
    #endif
    
    // set locals and globals from the calling context
    mp_locals_set(args->dict_locals);
    mp_globals_set(args->dict_globals);
    m_del_obj(callback_deal_thread_entry_args_t, args);
    
    MP_THREAD_GIL_ENTER();

    //mp_thread_start must placed after GIL_ENTER
    // signal that we are set up and running
    mp_thread_start();
 
    while(1)
    {
        mp_obj_t arg = NULL;
        MP_THREAD_GIL_EXIT();
        qpy_callbackdeal_trace("wait msg");
        int ret = Helios_MsgQ_Get(qpy_callback_deal_queue, (void*)&msg, sizeof(msg), HELIOS_WAIT_FOREVER);
        qpy_callbackdeal_trace("get msg");
        MP_THREAD_GIL_ENTER();
        if (ret == 0)
        {
    		//Each is processed according to the callback ID
    		qpy_callbackdeal_trace("callback_type_id:%d", msg.callback_type_id);
            switch(msg.callback_type_id)
            {
                case CALLBACK_TYPE_ID_EXTINT:
                {
                    st_CallBack_Extint *cb_msg = (st_CallBack_Extint *)msg.msg;
                    mp_obj_tuple_t *pair = MP_OBJ_TO_PTR(cb_msg->callback.arg);
                    pair->items[0] = mp_obj_new_int(cb_msg->pin_no);
                    pair->items[1] = mp_obj_new_int(cb_msg->edge);
                    // mp_obj_t extint_list[2] = {
				    //     mp_obj_new_int(cb_msg->pin_no),
				    //     mp_obj_new_int(cb_msg->edge),
			        // };
			        // arg = mp_obj_new_list(2, extint_list);
		            mp_sched_schedule_ex(&cb_msg->callback,  cb_msg->callback.arg);
                }
                break;
                case CALLBACK_TYPE_ID_AUDIO_RECORD:
                {
                    st_CallBack_AudioRecord *cb_msg = (st_CallBack_AudioRecord *)msg.msg;
                    mp_obj_t audio_cb[3] = {
            			mp_obj_new_str(cb_msg->p_data,strlen(cb_msg->p_data)),
            			mp_obj_new_int(cb_msg->len),
            			mp_obj_new_int(cb_msg->res),
            		};
            		if(RECORE_TYPE_FILE == cb_msg->record_type && NULL != cb_msg->p_data) {
            		    free(cb_msg->p_data);
            		}
            		arg = mp_obj_new_list(3, audio_cb);
                    mp_sched_schedule_ex(&cb_msg->callback,  arg);
				#if defined(PLAT_Unisoc) || defined(PLAT_Unisoc_8910_R05) || defined(PLAT_Unisoc_8910_R06)
					if (RECORE_TYPE_STREAM == cb_msg->record_type)
					{
						MP_THREAD_GIL_EXIT();
						Helios_msleep(10);
						MP_THREAD_GIL_ENTER();
					}
				#endif
                }
                break;
                case CALLBACK_TYPE_ID_VOICECALL_RECORD:
                {
                    st_CallBack_AudioRecord *cb_msg = (st_CallBack_AudioRecord *)msg.msg;
                    mp_obj_t audio_cb[3] = {
            			mp_obj_new_str(cb_msg->p_data,strlen(cb_msg->p_data)),
            			mp_obj_new_int(cb_msg->len),
            			mp_obj_new_int(cb_msg->res),
            		};

            		arg = mp_obj_new_list(3, audio_cb);
                    mp_sched_schedule_ex(&cb_msg->callback,  arg);
                }
                break;
                #if MICROPY_QPY_MODULE_QUECIOT
                case CALLBACK_TYPE_ID_QUETCH_CALLCB:
                 {
                    st_CallBack_Queth cb_msg;
                    memcpy(&cb_msg,msg.msg,sizeof(st_CallBack_Queth));
                    mp_obj_t tuple[] =
			        {
				        mp_obj_new_int_from_uint(cb_msg.event),
				        mp_obj_new_int_from_uint(cb_msg.errcode),
				        mp_obj_new_bytes(cb_msg.valueT, cb_msg.valLen)
                    };
                    if ( cb_msg.valueT != NULL  )
                        free(cb_msg.valueT);
                    free(msg.msg);
		            mp_sched_schedule_ex(&(cb_msg.call_cb),mp_obj_new_tuple(3, tuple));
                }
                break;
                #endif //end MICROPY_QPY_MODULE_QUECIOT
                #if MICROPY_QPY_MODULE_POC
                case CALLBACK_TYPE_ID_POC:
                {
                    qpy_poc_msg_proc(msg.msg);
                }
                break;
                #endif //end MICROPY_QPY_MODULE_POC
                #if MICROPY_QPY_MACHINE_KEYPAD
                case CALLBACK_TYPE_ID_KEYPAD:
                {
                    st_CallBack_Keypad *cb_msg = (st_CallBack_Keypad *)msg.msg;
                    mp_obj_t keypad_list[3] = 
                    {
				        mp_obj_new_int(cb_msg->event_id),
				        mp_obj_new_int(cb_msg->row),
				        mp_obj_new_int(cb_msg->col),
			        };
                    arg = mp_obj_new_list(3, keypad_list);
                    mp_sched_schedule_ex(&cb_msg->callback,  arg);

                }
                break;
                #endif
				#if MICROPY_QPY_SENSOR_ICC_KEY
				case CALLBACK_TYPE_ID_ICCKEY:
				{
					st_CallBack_IccKey *cb_msg = (st_CallBack_IccKey *)msg.msg;
                    mp_sched_schedule_ex(&cb_msg->callback,  mp_obj_new_int(cb_msg->key_event));
				}
                break;
				#endif
				
				#if MICROPY_QPY_MACHINE_KEY
				case CALLBACK_TYPE_ID_KEY:
				{
					st_CallBack_Key *cb_msg = (st_CallBack_Key *)msg.msg;
                    mp_obj_t key_list[2] = {
				        mp_obj_new_int(cb_msg->gpio_number),
				        mp_obj_new_int(cb_msg->key_event),
			        };
			        arg = mp_obj_new_list(2, key_list);
		            mp_sched_schedule_ex(&cb_msg->callback,  arg);
				}
				break;
				#endif
                #if MICROPY_QPY_SENSOR_HANDMIC_KEY
				case CALLBACK_TYPE_ID_HANDMIC_KEY:
				{
					st_CallBack_Handmic *cb_msg = (st_CallBack_Handmic *)msg.msg;
                    mp_obj_t key_list[2] = {
				        mp_obj_new_int(cb_msg->key_state),
				        mp_obj_new_int(cb_msg->key_value),
			        };
			        arg = mp_obj_new_list(2, key_list);
		            mp_sched_schedule_ex(&cb_msg->callback,  arg);
				}
                break;
				#endif
                #if MICROPY_QPY_MODULE_EXTUART_WK2114
				case CALLBACK_TYPE_ID_WK2114:
				{
					st_CallBack_Wk2114 *cb_msg = (st_CallBack_Wk2114 *)msg.msg;
                    mp_obj_t key_list[3] = {
				        mp_obj_new_int(cb_msg->int_type),
				        mp_obj_new_int(cb_msg->port),
                        mp_obj_new_int(cb_msg->read_lenth),
			        };
			        arg = mp_obj_new_list(3, key_list);
		            mp_sched_schedule_ex(&cb_msg->callback,  arg);
				}
                break;
				#endif
                #if MICROPY_QPY_MODULE_JRTC
                case CALLBACK_TYPE_ID_JRTC:
                {
                    st_CallBack_jrtc *cb_msg = (st_CallBack_jrtc *)msg.msg;
                    arg = mp_obj_new_int(cb_msg->result);
		            mp_sched_schedule_ex(&cb_msg->callback,  arg);

                }
                break;
				#endif
                #if MICROPY_QPY_MODULE_GPIO_ENCODER
				case CALLBACK_TYPE_ID_ENCODER:
				{
					st_CallBack_Encoder *cb_msg = (st_CallBack_Encoder *)msg.msg;
			        arg = mp_obj_new_int(cb_msg->spin_direct);
		            mp_sched_schedule_ex(&cb_msg->callback,  arg);
				}
                break;
				#endif
                #if MICROPY_QPY_MODULE_EXTGPIO_AW9523
				case CALLBACK_TYPE_ID_AW9523:
				{
                    st_CallBack_AW9523 *cb_msg = (st_CallBack_AW9523 *)msg.msg;
                    mp_obj_t aw9523_list[2] = {
				        mp_obj_new_int(cb_msg->pin_no),
				        mp_obj_new_int(cb_msg->pin_level),
			        };
			        arg = mp_obj_new_list(2, aw9523_list);
		            mp_sched_schedule_ex(&cb_msg->callback,  arg);
				}
                break;
				#endif
                case CALLBACK_TYPE_ID_TP:
				{
                    st_CallBack_TP *cb_msg = (st_CallBack_TP *)msg.msg;
                    mp_sched_schedule_ex(&cb_msg->callback,  mp_obj_new_int(cb_msg->state));
				}
                break;
			#if MICROPY_QPY_MODULE_BT
				case CALLBACK_TYPE_ID_BT:
				{
					qpy_bt_msg_proc(msg.msg);
					break;
				}
			#endif
			#if MICROPY_QPY_MODULE_BLE
				case CALLBACK_TYPE_ID_BLE:
				{
					qpy_ble_msg_proc(msg.msg);
					break;
				}
			#endif
			    case CALLBACK_TYPE_ID_LOAD_BOUNDMETH:
			    {
			        st_CallBack_LoadBoundMeth *cb_msg = (st_CallBack_LoadBoundMeth *)msg.msg;
                    qpy_callbackdeal_trace("cb_msg:0x%x", cb_msg);
                    qpy_callback_para_node_delete(cb_msg);
			        
			        mp_obj_t cb = mp_load_attr(cb_msg->callback->method_self, cb_msg->callback->method_name);
			        qpy_callbackdeal_trace("cb:0x%x", cb);
                    mp_call_function_1_protected(cb, cb_msg->arg);
			        break;
			    }
            #if MICROPY_QPY_MODULE_ALIPAY
                case CALLBACK_TYPE_ID_ALIPAY:
                {
                    st_CallBack_ALIPAY *alipay_msg = (st_CallBack_ALIPAY *)msg.msg;
                    mp_obj_t alipay_cb[5] = 
                    {
                        mp_obj_new_int(alipay_msg->cmd),
		                mp_obj_new_int(alipay_msg->rsp_code),
	 	                mp_obj_new_str((char *)(alipay_msg->result), (char *)(strlen(alipay_msg->result))),
                        mp_obj_new_int(alipay_msg->result_len),
                        mp_obj_new_str((char *)(alipay_msg->userdata), (char *)(strlen(alipay_msg->userdata))),
                    };
                    mp_sched_schedule_ex(&alipay_msg->callback, mp_obj_new_tuple(5, alipay_cb));
                    break;
                }
            #endif
            #if MICROPY_QPY_LPM_WAKEUP
               case CALLBACK_TYPE_ID_WAKEUP:
               {
                    st_CallBack_Wakeup *cb_msg = (st_CallBack_Wakeup *)msg.msg;
                    mp_sched_schedule_ex(&cb_msg->callback, mp_obj_new_int(cb_msg->level));
                    break;
               }
            #endif
			#if MICROPY_QPY_MACHINE_UART
               case CALLBACK_TYPE_ID_UART:
               {
                    st_CallBack_Uart *uart_msg = (st_CallBack_Uart *)msg.msg;
                    // mp_obj_t uart_cb[3] = 
                    // {
					// 	mp_obj_new_int(uart_msg->ind_type),
					// 	mp_obj_new_int(uart_msg->port),
					// 	mp_obj_new_int(uart_msg->size),
                    // };
					// arg = mp_obj_new_list(3,uart_cb);
                    mp_obj_tuple_t *pair = MP_OBJ_TO_PTR(uart_msg->callback.arg);
                    pair->items[0] = mp_obj_new_int(uart_msg->ind_type);
                    pair->items[1] = mp_obj_new_int(uart_msg->port);
                    pair->items[2] = mp_obj_new_int(uart_msg->size);
                    mp_sched_schedule_ex(&uart_msg->callback, uart_msg->callback.arg);
                    break;
               }
            #endif
            #if MICROPY_QPY_MODULE_NET
               case CALLBACK_TYPE_ID_NET:
               {    
                    qpy_callbackdeal_trace("callback_type_id:CALLBACK_TYPE_ID_NET = %d", msg.callback_type_id);
                    st_CallBack_Net *net_msg = (st_CallBack_Net *)msg.msg;
                    mp_obj_tuple_t *pair = MP_OBJ_TO_PTR(net_msg->callback.arg);
			        pair->items[0] = mp_obj_new_int(net_msg->event_id);
   			        pair->items[1] = mp_obj_new_int(net_msg->status);
			        pair->items[2] = mp_obj_new_int(net_msg->lac);
			        pair->items[3] = mp_obj_new_int(net_msg->cid);
			        pair->items[4] = mp_obj_new_int(net_msg->act);

                    mp_sched_schedule_ex(&net_msg->callback, net_msg->callback.arg);
                    break;
               }
            #endif
			#if MICROPY_QPY_MODULE_DATACALL
               case CALLBACK_TYPE_ID_DATACALL:
               {
                    st_CallBack_Datacall *datacall_msg = (st_CallBack_Datacall *)msg.msg;
                    mp_obj_tuple_t *pair = MP_OBJ_TO_PTR(datacall_msg->callback.arg);
			        pair->items[0] = mp_obj_new_int(datacall_msg->profile_idx);
   			        pair->items[1] = mp_obj_new_int(datacall_msg->nw_status);
			        pair->items[2] = mp_obj_new_int(datacall_msg->sim_id);

                    mp_sched_schedule_ex(&datacall_msg->callback, datacall_msg->callback.arg);
                    break;
               }
            #endif
            #if MICROPY_QPY_MODULE_ALIPAY_COLLECTION
               case CALLBACK_TYPE_ID_ALIPAY_COLLECTION:
               {
                    st_CallBack_ALIPAY_COLLECTION *alipay_collection_msg = (st_CallBack_ALIPAY_COLLECTION *)msg.msg;
                    mp_obj_t alipay_collection_cb[2] = 
                    {
						mp_obj_new_int(alipay_collection_msg->msgID),
						mp_obj_new_int(alipay_collection_msg->data),
                    };
                    mp_sched_schedule_ex(&alipay_collection_msg->callback, mp_obj_new_tuple(2, alipay_collection_cb));
                    break;
               }
            #endif
			#if MICROPY_QPY_MODULE_SMS
				case CALLBACK_TYPE_ID_SMS:
				{
					st_CallBack_SMS *sms_msg = (st_CallBack_SMS *)msg.msg;
                    mp_obj_t sms_cb[3] = 
                    {
                        mp_obj_new_int(sms_msg->sim_id),
		    			mp_obj_new_int(sms_msg->index),
		    			mp_obj_new_str(sms_msg->storage, strlen(sms_msg->storage))
                    };
                    mp_sched_schedule_ex(&sms_msg->callback, mp_obj_new_tuple(3, sms_cb));
					break;
				}
			#endif
            #if MICROPY_QPY_MODULE_SIF
				case CALLBACK_TYPE_ID_SIF:
				{
					st_CallBack_SIF *sif_msg = (st_CallBack_SIF *)msg.msg;
                    mp_sched_schedule_ex(&sif_msg->callback, mp_obj_new_bytes(sif_msg->data, sif_msg->data_len));
					break;
				}
			#endif
		#if defined(BOARD_BC32RA) || defined(BOARD_BC92RB) || defined(PLAT_SONY_ALT1350)//forrest.liu@20231228 add for FAE-102027 dump issue
			#if MICROPY_QPY_MACHINE_TIMER
               case CALLBACK_TYPE_ID_TIMER:
               {
                    st_CallBack_Timer *timer_msg = (st_CallBack_Timer *)msg.msg;

                    mp_sched_schedule_ex(&timer_msg->callback,  mp_const_none);
                    break;
               }
            #endif
		#endif
		#if defined(BOARD_EC600MCN_LA_POC_XBND) || defined(BOARD_EC600MCN_LE_POC_XBND) || defined(BOARD_EC600MEU_LA_POC_XBND) \
         || defined(BOARD_EC600MLA_LA_POC_XBND)
			   case CALLBACK_TYPE_ID_POCSPEAK:
               {
                    st_CallBack_PocSpeak *pocspeak_msg = (st_CallBack_PocSpeak *)msg.msg;
                    mp_obj_t pocspeak_list[2] = 
                    {
				        mp_obj_new_int(pocspeak_msg->speak_op),
				        mp_obj_new_int(pocspeak_msg->speak_result),
			        };
                    arg = mp_obj_new_list(2, pocspeak_list);
                    mp_sched_schedule_ex(&pocspeak_msg->callback,  arg);
                    break;
               }
		#endif
#if MICROPY_QPY_MODULE_NFC
				case CALLBACK_TYPE_ID_NFC:
				{
					st_CallBack_nfc *nfc_msg = (st_CallBack_nfc *)msg.msg;
					
					mp_obj_t nfc_list[2] = {0};
					nfc_list[0] = mp_obj_new_int(nfc_msg->oid);

					if(nfc_msg->data && nfc_msg->data_len != 0)
					{
						nfc_list[1] = mp_obj_new_bytes(nfc_msg->data, nfc_msg->data_len);
						arg = mp_obj_new_list(2, nfc_list);
					} else {
						arg = mp_obj_new_list(1, nfc_list);
					}
					mp_sched_schedule_ex(&nfc_msg->callback, arg);

					if(nfc_msg->data) {
						free(nfc_msg->data);
						nfc_msg->data = NULL;
					}
					
					break;
				}
#endif
#if MICROPY_QPY_SENSOR_VC9202

				case CALLBACK_TYPE_ID_VC9202:
				{
					st_CallBack_Sensor_VC9202 *cb_msg = (st_CallBack_Sensor_VC9202 *)msg.msg;
					mp_obj_t vc9202_list[3] = {
						mp_obj_new_int(cb_msg->work_mode),
						mp_obj_new_int(cb_msg->value),
						mp_obj_new_int(cb_msg->measure_mode),
					};
					arg = mp_obj_new_list(3, vc9202_list);
					mp_sched_schedule_ex(&cb_msg->callback,  arg);
				}
					break;
#endif
#if MICROPY_QPY_MODULE_LWM2M
                case CALLBACK_TYPE_ID_LWM2M:{
                    qpy_lwm2m_msg_proc(msg.msg);
                    break;
                }
#endif
#if MICROPY_QPY_MODULE_ESIM && MICROPY_QPY_MODULE_ESIM_IPA
                case CALLBACK_TYPE_ID_ESIM_IPA:{
                    qpy_esim_msg_proc(msg.msg);
                    break;
                }
#endif
                case CALLBACK_TYPE_ID_NONE:
                default:
                break;
            }
			
        #if defined(BOARD_BC32RA) || defined(BOARD_BC92RB) || defined(PLAT_SONY_ALT1350)//forrest.liu@20231228 modify for FAE-102027 dump issue
			if(msg.callback_type_id != CALLBACK_TYPE_ID_TIMER)
			{
				if(msg.msg) {
		            free(msg.msg);
		        }
			}
			memset(&msg, 0, sizeof(st_CallBack_Deal));
		#else
            if(msg.msg) {
	            free(msg.msg);
                msg.msg = NULL;
	        }
            memset(&msg, 0, sizeof(st_CallBack_Deal));
		#endif
	        
		}
		else
		{
#if defined(BOARD_BC32RA) || defined(BOARD_BC92RB) || defined(PLAT_SONY_ALT1350)//forrest.liu@20231228 modify for FAE-102027 dump issue
			if(msg.callback_type_id != CALLBACK_TYPE_ID_TIMER)
			{
				if(msg.msg) {
					free(msg.msg);
				}
			}
			memset(&msg, 0, sizeof(st_CallBack_Deal));
#else
			if(msg.msg) {
				free(msg.msg);
			}
			memset(&msg, 0, sizeof(st_CallBack_Deal));
#endif
		}
    }
}

#endif

MP_REGISTER_ROOT_POINTER(void * loadmeth_para);
