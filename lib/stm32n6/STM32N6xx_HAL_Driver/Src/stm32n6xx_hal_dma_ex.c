/**
  **********************************************************************************************************************
  * @file    stm32n6xx_hal_dma_ex.c
  * @author  MCD Application Team
  * @brief   DMA Extension HAL module driver
  *          This file provides firmware functions to manage the following functionalities of the DMA extension
  *          peripheral:
  *           + Linked-List Initialization and De-Initialization Functions
  *           + Linked-List I/O Operation Functions
  *           + Linked-List Management Functions
  *           + Data Handling, Repeated Block and Trigger Configuration Functions
  *           + Suspend and Resume Operation Functions
  *           + FIFO Status Function
  *
  **********************************************************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **********************************************************************************************************************
  @verbatim
  ======================================================================================================================
                                 ##### How to use this driver #####
  ======================================================================================================================
    [..]
      Alternatively to the normal programming mode, a DMA channel can be programmed by a list of transfers, known as
      linked-list (list of Node items). Each node is defined by its data structure.
      Each node specifies a standalone DMA channel.
      When enabled, the DMA channel fetch the first linked-list node from SRAM (known as head node). When executed, the
      next linked list node will be fetched and executed. This operation is repeated until the end of the whole
      linked-list queue. Optionally, the linked-list can be linear where the last linked-list queue node is not linked
      to another queue node or circular where the last linked-list node is linked to any linked-list queue node.

          (+) Linear linked-list:
              The DMA channel fetch and execute all DMA linked-list queue from first node (head node) to last node
              (tail node) ones. When the last node is completed, the DMA channel remains in idle state and another
              transfer can be lunched.

          (+) Circular linked-list:
              The DMA channel fetch and execute all DMA linked-list queue from first node (head node) to last node (tail
              node). When last node is executed, the DMA channel fetches the first circular node another time and repeat
              the same sequence in an infinite loop (Circular transfer). To stop the DMA channel, an abort operation is
              required. This linked-list mode replaces the legacy circular transfers.

    [..]
      In order to reduce linked-list queue executing time and power consumption, the DMA channel supports executing the
      dynamic linked-list format. In fact, the DMA supports the execution of 2 types of linked-list formats : static and
      dynamic.

          (+) Static linked-list:
              The static linked-list format refers to the full linked-list node where all DMA channel parameters are
              fetched and executed independently of the redundancy of information.

          (+) Dynamic linked-list:
              The dynamic linked-list format refer to the customized linked-list node where only DMA channel necessary
              parameters are fetched and executed (Example: data size = 20 on previous node, and data size = 20 on the
              current node => No need to update it).

      For linked-list transfers, the DMA channel can execute the linked-list queue node by node. This feature is named
      link step mode. When activated, enabling the DMA channel first time allows to fetch the head node from memory
      then it stops. Then, another DMA channel enable is needed to execute the node. After that, keeping enabling the
      DMA channel is needed to execute each node until the end of linked-list queue. When the linked-list queue is
      circular, enabling the DMA channel in an infinite loop is required to keep the DMA channel running. This feature
      is useful for debug purpose or asynchronously executing queue nodes.

    [..]
      Each DMA channel transfer (normal or linked-list), is highly configurable according to DMA channel instance
      integrated in devices. These configuration can be :

          (+) Repeated block configuration :
              If the feature is supported, the DMA channel can performs a repeated block transfers. Named also 2
              dimension addressing transfers, this feature can transfer n iteration of programmed block transfer (Block
              transfer is the legacy data size). Additional to the repeat count of a block, DMA channel addresses can
              jump after at burst and block level. The jump length is a programmable parameter defined by DMA user.
               (++) Jump at burst level :
                    The DMA channel keep an empty area, between each 2 consecutive bursts transmitted.
               (++) Jump at block level :
                    The DMA channel keep an empty area, between each 2 consecutive blocks transmitted.

          (+) Trigger :
              The DMA channel transfers can be conditioned by hardware signals edges (rising or falling) named hardware
              triggers. Trigger condition can be applied at :
               (++) Single/Burst level :
                    Each single/burst data transmission is conditioned by a signal trigger hit.
               (++) Block level :
                    Each block data transmission is conditioned by a signal trigger hit.
               (++) Repeated block level :
                    Each repeated block data transmission is conditioned by a signal trigger hit.
               (++) Node level :
                    Each node execution is conditioned by a signal trigger hit.
              The DMA channel can report a trigger overrun when detects more than 2 trigger signal edges before
              executing the current transfer.

          (+) Data handling :
              The data handling feature is a FIFO capability that can be :
                (++) Padding pattern :
                     Padding selected pattern (zero padding or sign extension) when the source data width is smaller
                     than the destination data width at single level.
                (++) Truncation :
                     Truncate section from the source data single when the source data width is bigger than the
                     destination data width.
                (++) Pack/Unpack :
                     Pack a set of data when source data width is smaller than the destination data width.
                     Unpack a set of data when source data width is bigger than the destination data width.
                (++) Exchange :
                     Exchange data at byte, half-word and word on the destination and at byte level on the source.

    [..]
      Each DMA channel transfer (normal or linked-list) when it is active, can be suspended and resumed at run time
      application. When trying to suspend an ongoing transfer, the DMA channel isn't suspended instantly but complete
      the current ongoing single/burst then it stops.
      When the DMA channel is suspended, the current transfer can be resumed instantly.

    [..]
      The DMA channel that supports FIFO, can report in real time the number of beats remains on destination (Output)
      FIFO level.

    *** Linked-List Initialization and De-Initialization operation ***
    ==================================================================
    [..]
      Differently from normal transfers, DMA channel initialization and de-initialization need less parameters as the
      remaining transfer parameters are defined by linked-list nodes.

          (+) Use HAL_DMAEx_List_Init() to initialize a DMA channel in linked-list mode according to programmed fields.
              When called, the DMA channel will be ready to execute linked-list queues.

          (+) Use HAL_DMAEx_List_DeInit() to de-initialize a DMA channel in linked-list mode.
              When called, the DMA channel will be in reset. It is mandatory to reinitialize it for next transfer.

    *** Linked-List I/O Operation ***
    =================================
    [..]
          (+) Use HAL_DMAEx_List_Start() to start a DMA transfer in linked-list mode after the configuration of
              linked-list queue base address and offset in polling mode (Blocking mode).

          (+) Use HAL_DMAEx_List_Start_IT() to start a DMA transfer in linked-list mode after the configuration of
              linked-list queue base address and offset in interrupt mode (Non-blocking mode).

    *** Linked-List Management ***
    ==============================
    [..]
      The linked-list management is a software processing independently of DMA channel hardware. It allows to reset,
      build, create, insert, remove, replace, circularize, convert both nodes and queue in order to perform DMA
      channel transfers in linked-list mode.
      Linked-list APIs and types are adapted to reduce memory footprint.

    *** Linked-list nodes building ***
    [..]
      At node level, the operations that can be done are building a new linked-list node or get a linked-list node
      information from a built node. The linked-list nodes have two forms according to 2 dimensions addressing
      capability. The linear addressing nodes contains the information of all DMA channel features except the 2
      dimension addressing features and the 2 dimensions addressing nodes contain the information of all available
      features.

          (+) Use HAL_DMAEx_List_BuildNode() to build the DMA linked-list node according to the specified parameters.
              Build operation allow to convert the specified parameter in values known by the DMA channel and place them
              in memory.
              Placing DMA linked-list in SRAM must be done in accordance to product specification to ensure that the
              link access port can access to the specified SRAM.
              (++) The DMA linked-list node parameter address should be 32bit aligned and should not exceed the 64 KByte
              addressable space.

          (+) Use HAL_DMAEx_List_GetNodeConfig() to get the specified configuration parameter on building node.
              This API can be used when need to change few parameter to build new node.

    *** Inserting nodes to linked-list queue ***
    [..]
      In order to build a sequence of DMA transaction with different configuration, we need to insert built node at
      linked-list queue (node present an elementary DMA transaction) in linked-list queue on any position to have the
      full flexibility of ordering nodes or extend the sequence of queue transactions.

          (+) Use HAL_DMAEx_List_InsertNode() to insert new built node in any queue position of linked-list queue
              according to selecting previous node. When calling this API with previous node parameter is NULL, the
              inserted node will be placed at the head of the linked-list queue.
              (++) This API must be used after HAL_DMAEx_List_BuildNode() otherwise an error will be returned.
              (++) This API must be called for static queues format.
              (++) This API shall be avoided when adding new node at the head or the tail of queue (overhead of
                   footprint and performance : use HAL_DMAEx_List_InsertNode_Head() or HAL_DMAEx_List_InsertNode_Tail()
                   instead).

          (+) Use HAL_DMAEx_List_InsertNode_Head() to insert new built node at the head of linked-list queue. The head
              node will not be overwritten but will be the second queue node.
              (++) This API must be used after HAL_DMAEx_List_BuildNode() otherwise an error will be returned.
              (++) This API must be called for static queues format.

          (+) Use HAL_DMAEx_List_InsertNode_Tail() to insert new built node at the tail of linked-list queue. The tail
              node will not be overwritten but will be the penultimate queue node.
              (++) This API must be used after HAL_DMAEx_List_BuildNode() otherwise an error will be returned.
              (++) This API must be called for static queues format.

    *** Removing nodes from linked-list queue ***
    [..]
      There is some cases when removing a node from linked-list queue is needed (need to remove an elementary DMA
      transaction). Removing node allows to unlink a node from DMA linked-list queue (NOT DELETED), so the removed node
      can be reused for another queue or to be added to the same queue without need to rebuild it in next step.

          (+) Use HAL_DMAEx_List_RemoveNode() to remove any yet built and inserted node from linked-list queue according
              to selected node.
              (++) This API must be called for static queues format.
              (++) This API shall be avoided when removing the head or the tail of linked-list queue (overhead of
                   footprint and performance : use HAL_DMAEx_List_RemoveNode_Head() or HAL_DMAEx_List_RemoveNode_Tail()
                   instead).

          (+) Use HAL_DMAEx_List_RemoveNode_Head() to remove the head node from linked-list queue.
              (++) This API must be called for static queues format.

          (+) Use HAL_DMAEx_List_RemoveNode_Tail() to remove the tail node from linked-list queue.
              (++) This API must be called for static queues format.

    *** Replacing nodes on linked-list queue ***
    [..]
      There is some cases when replacing a node from linked-list queue is needed (need to replace an elementary DMA
      transfer, by another one that have not the same configuration). Replacing node allows to unlink the node to be
      replaced from DMA linked-list queue (NOT DELETED) and link instead a new node. So the replaced node can be reused
      for another queue or to be added to the same queue without need to rebuild it in next step and the new node cannot
      be reused except when remove it or replaced in next step.

          (+) Use HAL_DMAEx_List_ReplaceNode() to replace any yet built and inserted node on linked-list queue according
              to selected node.
              (++) This API must be called for static queues format.
              (++) This API shall be avoided when replacing the head or the tail linked-list queue (overhead of
                   footprint and performance : use HAL_DMAEx_List_ReplaceNode_Head() or
                   HAL_DMAEx_List_ReplaceNode_Tail() instead).

          (+) Use HAL_DMAEx_List_ReplaceNode_Head() to replace the head node of linked-list queue.
              (++) This API must be called for static queues format.

          (+) Use HAL_DMAEx_List_ReplaceNode_Tail() to replace the tail node from linked-list queue.
              (++) This API must be called for static queues format.

    *** Reset linked-list queue ***
    [..]
      After finishing using a linked-list queue, it can be reset and cleared and it's content nodes will be
      unlinked (NOT DELETED) and reused on another queue.

          (+) Use HAL_DMAEx_List_ResetQ() to reset a linked-list queue and unlink all it's content nodes.
              (++) This API must be called for ready state queues.
              (++) This API must be called for static queues format.

    *** Inserting linked-list queue ***
    [..]
      To ensure the flexibility of building linked-list queue by their targeted functionalities (Example: 3 nodes for
      action 1 and 5 nodes for action 2), it is possible to build a queue for action 1 that contains action 1 nodes and
      a queue for action 2 that contains action 2 nodes then concatenating the 2 queues. So, there are some cases where
      the management of linked-list at queue granularity is needed.

          (+) Use HAL_DMAEx_List_InsertQ() to insert source linked-list queue to a destination linked-list queue
              according to selecting previous node.
              (++) This API must be called for static queues format.
              (++) This API shall be avoided when inserting source linked-list queue at the head or the tail of
                   destination queue (overhead of footprint and performance : use HAL_DMAEx_List_InsertQ_Head() or
                   HAL_DMAEx_List_InsertQ_Tail() instead).

          (+) Use HAL_DMAEx_List_InsertQ_Head() to insert a source linked-list queue at the head of linked-list
              destination queue.
              (++) This API must be called for static queues format.

          (+) Use HAL_DMAEx_List_InsertQ_Tail() to insert a source linked-list queue at the tail of linked-list
              destination queue.
              (++) This API must be called for static queues format.

    *** Circularizing linked-list queue ***
    [..]
      In order to perform tasks in infinite loop with DMA channel, it is possible to circularize the linked-list queues.
      Circularizing queue allows to link last linked-list queue node to any previous node of the same queue (This node
      is named first circular queue). When the first circular node is the head node, all linked-list queue nodes will be
      executed in infinite loop. When the first circular node is not the head nodes, all precedent nodes are executed
      once and all remaining nodes are executed in an infinite loop.

          (+) Use HAL_DMAEx_List_SetCircularModeConfig() to circularize the linked-list queue according to first
              circular node selected.
              (++) This API must be called for static queues format.
              (++) This API shall be avoided when first circular node is the head linked-list queue node (overhead of
                   footprint and performance : use HAL_DMAEx_List_SetCircularMode() instead).

          (+) Use HAL_DMAEx_List_SetCircularMode() to circularize the linked-list queue with linking last queue node
              with first queue node.
              (++) This API must be called for static queues format.

          (+) Use HAL_DMAEx_List_ClearCircularMode() to clear any linked-list queue circular configuration.
              (++) This API must be called for static queues format.


    *** Converting linked-list queue ***
    [..]
      To have the best DMA channel linked-list queue execution, it is recommended to convert yet build linked-list queue
      to dynamic format (Static is the default format). When linked-list queue becomes dynamic, all queue nodes are
      optimized and only changed parameters will be updated between nodes. So, the DMA will fetch only changes
      parameters instead of the whole node.

          (+) Use HAL_DMAEx_List_ConvertQToDynamic() to convert a linked-list queue to dynamic format.
              (++) This API must be called for ready state queues.
              (++) This API must be called for static queues format.
              (++) This API must be called as the last API before starting the DMA channel in linked-list mode.

          (+) Use HAL_DMAEx_List_ConvertQToStatic() to convert a linked-list queue to static format.
              (++) This API must be called for ready state queues.
              (++) This API must be called for dynamic queues format.
              (++) This API must be called as the first API after the full execution of linked-list queue when the
                   execution mode is linear (not circular) if it is dynamic and a linked-list queue management is
                   needed.
              (++) This API must be called as the first API after the aborting the execution of the current linked-list
                   queue when the execution mode is linear (not circular) if it is dynamic and a linked-list queue
                   management is needed.

    [..]
      When converting a circular queue to dynamic format and when the first circular node is the last queue node, it is
      recommended to duplicate the last circular node in order to ensure the full optimization when calling
      HAL_DMAEx_List_ConvertQToDynamic() API. In this case, updated information are only addresses which allow to reduce
      4 words of update for linear nodes per node execution and 6 words update for 2 dimensions addressing nodes per
      node execution.


    *** Linking linked-list queue to DMA channel ***
    [..]
      In order to have the possibility of the creation of an infinity queues (limited by available memory size), the
      building of linked-list queue is fully independent from DMA channels. It is possible to build all needed queues if
      their size is less then available memory at startup time, then linking each time when needed a linked-list queue
      to an idle DMA channel.

          (+) Use HAL_DMAEx_List_LinkQ() to link a ready linked-list queue to ready DMA channel.
              (++) This API supports the two format of linked-list (Static and dynamic).
              (++) This API must be called for ready state queues and DMA channels.

          (+) Use HAL_DMAEx_List_ConvertQToStatic() to unlink a ready linked-list queue to ready DMA channel.
              (++) This API supports the two format of linked-list (Static and dynamic).
              (++) This API must be called for ready state queues and DMA channels.

    *** User sequence ***
    [..]
      To use cleanly the DMA linked-list library, ensure to apply the following call sequences :

          (+) Linear transfer :
              Linked-list queue building
              (++) HAL_DMAEx_List_BuildNode()
              (++) HAL_DMAEx_List_InsertNode_Tail()
                              .
                              .
                              .
              (++) HAL_DMAEx_List_BuildNode()
              (++) HAL_DMAEx_List_InsertNode_Tail()
              (++) HAL_DMAEx_List_ConvertQToDynamic()
              Linked-list queue execution
              (++) HAL_DMAEx_List_Init()
              (++) HAL_DMAEx_List_LinkQ()
              (++) HAL_DMAEx_List_Start() / HAL_DMAEx_List_Start_IT()
              (++) HAL_DMAEx_List_UnLinkQ()
              (++) HAL_DMAEx_List_DeInit()

          (+) Circular transfer :
              Linked-list queue building
              (++) HAL_DMAEx_List_BuildNode()
              (++) HAL_DMAEx_List_InsertNode_Tail()
                              .
                              .
                              .
              (++) HAL_DMAEx_List_BuildNode()
              (++) HAL_DMAEx_List_InsertNode_Tail()
              (++) HAL_DMAEx_List_SetCircularModeConfig() / HAL_DMAEx_List_SetCircularMode()
              (++) HAL_DMAEx_List_ConvertQToDynamic()
              Linked-list queue execution
              (++) HAL_DMAEx_List_Init()
              (++) HAL_DMAEx_List_LinkQ()
              (++) HAL_DMAEx_List_Start() / HAL_DMAEx_List_Start_IT()
              (++) HAL_DMA_Abort() / HAL_DMA_Abort_IT()
              (++) HAL_DMAEx_List_UnLinkQ()
              (++) HAL_DMAEx_List_DeInit()


    *** Data Handling ***
    =====================
    [..]
      In order to avoid some CPU data processing in several cases, the DMA channel provides some features related to
      FIFO capabilities titled data handling.
                (++) Padding pattern
                     Padding selected pattern (zero padding or sign extension) when the source data width is smaller
                     than the destination data width at single level.
                     Zero padding       (Source : 0xABAB ------> Destination : 0xABAB0000)
                     Sign bit extension (Source : 0x0ABA ------> Destination : 0x00000ABA)
                                        (Source : 0xFABA ------> Destination : 0xFFFFFABA)
                (++) Truncation :
                     Truncate section from the source data single when the source data width is bigger than the
                     destination data width.
                     Left truncation  (Source : 0xABABCDCD ------> Destination : 0xCDCD)
                     Right truncation (Source : 0xABABCDCD ------> Destination : 0xABAB)
                (++) Pack/Unpack :
                     Pack a set of data when source data width is smaller than the destination data width.
                     Unpack a set of data when source data width is bigger than the destination data width.
                     Pack   (Source : 0xAB, 0xCD ------> Destination : 0xABCD)
                     UnPack (Source : 0xABCD     ------> Destination : 0xAB, 0xCD)
                (++) Exchange :
                     Exchange data at byte and half-word on the destination and at byte level on the source.
                     Considering source and destination are both word type. Exchange operation can be as follows.
                     In examples below, one exchange setting is enabled at a time.
                     Source byte exchange only (Source : 0xAB12CD34 ------> Destination : 0xABCD1234)
                     Destination byte exchange only (Source : 0xAB12CD34 ------> Destination : 0x12AB34CD)
                     Destination half-word exchange only (Source : 0xAB12CD34 ------> Destination : 0xCD34AB12)
                     In addition, in case of double-word, Exchange data at word level on the destination is also
                     available.
                     Considering source and destination are both double-word type.
                     word exchange only (Source : 0xAB12CD34EF567890 ------> Destination : 0xEF567890AB12CD34)

          (+) Use HAL_DMAEx_ConfigDataHandling() to configure data handling features. Previous elementary explained
              can be combined according to application needs.
              (++) This API is complementary of normal transfers.
              (++) This API must not be called for linked-list transfers as data handling information are configured at
                   node level.

    *** User sequence ***
    [..]
      To configure cleanly the DMA channel data handling, ensure to apply the following call sequence :

          (+) Linear transfer :
              (++) HAL_DMA_Init()
              (++) HAL_DMAEx_ConfigDataHandling()
              (++) HAL_DMA_Start()

    *** Repeated Block ***
    ======================
    [..]
      When available, this feature is used when the data size is higher then 65535 bytes (Maximum block size) or for
      scattering / gathering data.
                (++) Gather data
                     Source            Destination
                     0xAA              0xAA
                     0xBB              0xAA
                     0xAA      ==>     0xAA
                     0xCC
                     0xAA
                (++) Scatter data
                     Source            Destination
                     0xAA              0xAA
                     0xAA              0xBB
                     0xAA      ==>     0xAA
                                       0xBB
                                       0xAA

          (+) Use HAL_DMAEx_ConfigRepeatBlock() to configure data repeated block feature. Jump addresses and
              incrementing or decrementing on source and destination can be combined to have the need application
              behavior.
              (++) This API is complementary of normal transfers.
              (++) This API must not be called for linked-list transfers as repeated block information are configured at
                   node level.
              (++) This API must be called only for DMA channel that supports repeated block feature.

    *** User sequence ***
    [..]
      To configure cleanly the DMA channel repeated block, ensure to apply the following call sequence :

          (+) Linear transfer :
              (++) HAL_DMA_Init()
              (++) HAL_DMAEx_ConfigRepeatBlock()
              (++) HAL_DMA_Start()

    *** Trigger Configuration ***
    =============================
    [..]
      When application needs that DMA transfers are conditioned by internal or external events, the trigger feature can
      do that. Trigger signals are a set of device signal that are linked to DMA trigger inputs that allows to start the
      DMA transfers.
      To setup a trigger transfers, three DMA channel parameters are needed:

          (+) Trigger mode
              This parameter specifies the trig level.
               (++) Block level
               (++) Repeated block level
               (++) Node level
               (++) Single / Burst level

          (+) Trigger polarity
              This parameter specifies the DMA trigger sensitivity (Rising or falling).

          (+) Trigger selection
              This parameter specifies the DMA trigger hardware signal.

          (+) Use HAL_DMAEx_ConfigTrigger() to configure trigger feature.
              (++) This API is complementary to normal transfers APIs.
              (++) This API must not be called for linked-list transfers as trigger information are configured at
                   node level.

    *** User sequence ***
    [..]
      To configure cleanly the DMA channel trigger, ensure to apply the following call sequence :
          (+) Linear transfer :
              (++) HAL_DMA_Init()
              (++) HAL_DMAEx_ConfigTrigger()
              (++) HAL_DMA_Start()

    *** Suspend and resume operation ***
    ====================================
    [..]
      There are several cases when needs to suspend a DMA current transfer (Example: liberate bandwidth for more
      priority DMA channel transfer). Suspending DMA channel (same as abort) is available in polling (blocking mode) and
      interrupt (non-blocking mode) modes. When suspended, a DMA channel can be instantly resumed.

          (+) Use HAL_DMAEx_Suspend() to suspend an ongoing DMA channel transfer in polling mode (Blocking mode).

          (+) Use HAL_DMAEx_Suspend_IT() to suspend an ongoing DMA channel transfer in interrupt mode (Non-blocking
              mode).

          (+) Use HAL_DMAEx_Resume() to resume a suspended DMA channel transfer execution.

    *** FIFO status ***
    ===================
    [..]
      In several cases, the information of FIFO level is useful to inform at application level how to process remaining
      data. When not empty, the DMA channel FIFO cannot be flashed only by reset.

          (+) Use HAL_DMAEx_GetFifoLevel() to get the DMA channel FIFO level (available beats in FIFO).

    @endverbatim
  **********************************************************************************************************************
  */

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup DMAEx DMAEx
  * @brief DMA Extended HAL module driver
  * @{
  */

#ifdef HAL_DMA_MODULE_ENABLED

/* Private types -----------------------------------------------------------------------------------------------------*/
/* Private variables -------------------------------------------------------------------------------------------------*/
/* Private Constants -------------------------------------------------------------------------------------------------*/
/* Private macros ----------------------------------------------------------------------------------------------------*/
/* Private function prototypes ---------------------------------------------------------------------------------------*/
static void DMA_List_Init(DMA_HandleTypeDef const *const hdma);
static void DMA_List_BuildNode(DMA_NodeConfTypeDef const *const pNodeConfig,
                               DMA_NodeTypeDef *const pNode);
static void DMA_List_GetNodeConfig(DMA_NodeConfTypeDef *const pNodeConfig,
                                   DMA_NodeTypeDef const *const pNode);
static uint32_t DMA_List_CheckNodesBaseAddresses(DMA_NodeTypeDef const *const pNode1,
                                                 DMA_NodeTypeDef const *const pNode2,
                                                 DMA_NodeTypeDef const *const pNode3);
static uint32_t DMA_List_CheckNodesTypes(DMA_NodeTypeDef const *const pNode1,
                                         DMA_NodeTypeDef const *const pNode2,
                                         DMA_NodeTypeDef const *const pNode3);
static void DMA_List_GetCLLRNodeInfo(DMA_NodeTypeDef const *const pNode,
                                     uint32_t *const cllr_mask,
                                     uint32_t *const cllr_offset);
static uint32_t DMA_List_FindNode(DMA_QListTypeDef const *const pQList,
                                  DMA_NodeTypeDef const *const pNode,
                                  DMA_NodeInQInfoTypeDef *const NodeInfo);
static void DMA_List_ResetQueueNodes(DMA_QListTypeDef const *const pQList,
                                     DMA_NodeInQInfoTypeDef const *const NodeInfo);
static void DMA_List_FillNode(DMA_NodeTypeDef const *const pSrcNode,
                              DMA_NodeTypeDef *const pDestNode);
static void DMA_List_ConvertNodeToDynamic(uint32_t ContextNodeAddr,
                                          uint32_t CurrentNodeAddr,
                                          uint32_t RegisterNumber);
static void DMA_List_ConvertNodeToStatic(uint32_t ContextNodeAddr,
                                         uint32_t CurrentNodeAddr,
                                         uint32_t RegisterNumber);
static void DMA_List_UpdateDynamicQueueNodesCLLR(DMA_QListTypeDef const *const pQList,
                                                 uint32_t LastNode_IsCircular);
static void DMA_List_UpdateStaticQueueNodesCLLR(DMA_QListTypeDef const *const pQList,
                                                uint32_t operation);
static void DMA_List_FormatNode(DMA_NodeTypeDef *const pNode,
                                uint32_t RegisterIdx,
                                uint32_t RegisterNumber,
                                uint32_t Format);
static void DMA_List_ClearUnusedFields(DMA_NodeTypeDef *const pNode,
                                       uint32_t FirstUnusedField);
static void DMA_List_CleanQueue(DMA_QListTypeDef *const pQList);

/* Exported functions ------------------------------------------------------------------------------------------------*/

/** @addtogroup DMAEx_Exported_Functions
  * @{
  */

/** @addtogroup DMAEx_Exported_Functions_Group1
  *
@verbatim
  ======================================================================================================================
                 ##### Linked-List Initialization and De-Initialization Functions #####
  ======================================================================================================================
    [..]
      This section provides functions allowing to initialize and de-initialize the DMA channel in linked-list mode.
    [..]
      (+) The HAL_DMAEx_List_Init() function follows the DMA channel linked-list mode configuration procedures as
          described in reference manual.
      (+) The HAL_DMAEx_List_DeInit() function allows to de-initialize the DMA channel in linked-list mode.

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the DMA channel in linked-list mode according to the specified parameters in the
  *         DMA_InitLinkedListTypeDef and create the associated handle.
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_Init(DMA_HandleTypeDef *const hdma)
{
  /* Get tick number */
  uint32_t tickstart = HAL_GetTick();

  /* Check the DMA channel handle parameter */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_ALL_INSTANCE(hdma->Instance));
  assert_param(IS_DMA_PRIORITY(hdma->InitLinkedList.Priority));
  assert_param(IS_DMA_LINK_STEP_MODE(hdma->InitLinkedList.LinkStepMode));
  assert_param(IS_DMA_TCEM_LINKEDLIST_EVENT_MODE(hdma->InitLinkedList.TransferEventMode));
  assert_param(IS_DMA_LINKEDLIST_MODE(hdma->InitLinkedList.LinkedListMode));
  /* Check DMA channel instance */
  if ((IS_HPDMA_INSTANCE(hdma->Instance) != 0U) || (IS_GPDMA_INSTANCE(hdma->Instance) != 0U))
  {
    assert_param(IS_DMA_LINK_ALLOCATED_PORT(hdma->InitLinkedList.LinkAllocatedPort));
  }

  /* Allocate lock resource */
  __HAL_UNLOCK(hdma);

  /* Change DMA peripheral state */
  hdma->State = HAL_DMA_STATE_BUSY;

  /* Disable the DMA channel */
  __HAL_DMA_DISABLE(hdma);

  /* Check if the DMA channel is effectively disabled */
  while ((hdma->Instance->CCR & DMA_CCR_EN) != 0U)
  {
    /* Check for the Timeout */
    if ((HAL_GetTick() - tickstart) > HAL_TIMEOUT_DMA_ABORT)
    {
      /* Update error code */
      hdma->ErrorCode = HAL_DMA_ERROR_TIMEOUT;

      /* Change the DMA state */
      hdma->State = HAL_DMA_STATE_ERROR;

      return HAL_ERROR;
    }
  }

  /* Initialize the DMA channel registers */
  DMA_List_Init(hdma);

  /* Update DMA channel operation mode */
  hdma->Mode = hdma->InitLinkedList.LinkedListMode;

  /* Update the DMA channel error code */
  hdma->ErrorCode = HAL_DMA_ERROR_NONE;

  /* Update the DMA channel state */
  hdma->State = HAL_DMA_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  DeInitialize the DMA channel when it is configured in linked-list mode.
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_DeInit(DMA_HandleTypeDef *const hdma)
{

  /* Get DMA instance */
  DMA_TypeDef *p_dma_instance;

  /* Get tick number */
  uint32_t tickstart = HAL_GetTick();

  /* Check the DMA peripheral handle parameter */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_ALL_INSTANCE(hdma->Instance));


  /* Get DMA instance */
  p_dma_instance = GET_DMA_INSTANCE(hdma);

  /* Disable the selected DMA Channel */
  __HAL_DMA_DISABLE(hdma);

  /* Check if the DMA channel is effectively disabled */
  while ((hdma->Instance->CCR & DMA_CCR_EN) != 0U)
  {
    /* Check for the Timeout */
    if ((HAL_GetTick() - tickstart) > HAL_TIMEOUT_DMA_ABORT)
    {
      /* Update error code */
      hdma->ErrorCode = HAL_DMA_ERROR_TIMEOUT;

      /* Change the DMA state */
      hdma->State = HAL_DMA_STATE_ERROR;

      return HAL_ERROR;
    }
  }

  /* Reset DMA Channel registers */
  hdma->Instance->CCR   = 0U;
  hdma->Instance->CLBAR = 0U;
  hdma->Instance->CTR1  = 0U;
  hdma->Instance->CTR2  = 0U;
  hdma->Instance->CBR1  = 0U;
  hdma->Instance->CSAR  = 0U;
  hdma->Instance->CDAR  = 0U;
  hdma->Instance->CLLR  = 0U;

  /* Reset 2D Addressing registers */
  if (IS_DMA_2D_ADDRESSING_INSTANCE(hdma->Instance) != 0U)
  {
    hdma->Instance->CTR3 = 0U;
    hdma->Instance->CBR2 = 0U;
  }


  /* Clear privilege attribute */
  CLEAR_BIT(p_dma_instance->PRIVCFGR, (1UL << (GET_DMA_CHANNEL(hdma) & 0x1FU)));


#if defined (CPU_IN_SECURE_STATE)
  /* Clear secure attribute */
  CLEAR_BIT(p_dma_instance->SECCFGR, (1UL << (GET_DMA_CHANNEL(hdma) & 0x1FU)));
#endif /* CPU_IN_SECURE_STATE */

  /* Clear all flags */
  __HAL_DMA_CLEAR_FLAG(hdma, (DMA_FLAG_TC | DMA_FLAG_HT | DMA_FLAG_DTE | DMA_FLAG_ULE | DMA_FLAG_USE | DMA_FLAG_SUSP |
                              DMA_FLAG_TO));

  /* Clean all callbacks */
  hdma->XferCpltCallback     = NULL;
  hdma->XferHalfCpltCallback = NULL;
  hdma->XferErrorCallback    = NULL;
  hdma->XferAbortCallback    = NULL;
  hdma->XferSuspendCallback  = NULL;

  /* Check the linked-list queue */
  if (hdma->LinkedListQueue != NULL)
  {
    /* Update the queue state and error code */
    hdma->LinkedListQueue->State     = HAL_DMA_QUEUE_STATE_READY;
    hdma->LinkedListQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

    /* Clean DMA queue */
    hdma->LinkedListQueue = NULL;
  }

  /* Clean DMA parent */
  if (hdma->Parent != NULL)
  {
    hdma->Parent = NULL;
  }

  /* Update DMA channel operation mode */
  hdma->Mode = DMA_NORMAL;

  /* Update the DMA channel error code */
  hdma->ErrorCode = HAL_DMA_ERROR_NONE;

  /* Update the DMA channel state */
  hdma->State = HAL_DMA_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hdma);

  return HAL_OK;
}
/**
  * @}
  */

/** @addtogroup DMAEx_Exported_Functions_Group2
  *
@verbatim
  ======================================================================================================================
                         ##### Linked-List IO Operation Functions #####
  ======================================================================================================================
    [..]
      This section provides functions allowing to :
      (+) Configure to start DMA transfer in linked-list mode.

    [..]
      (+) The HAL_DMAEx_List_Start() function allows to start the DMA channel transfer in linked-list mode (Blocking
          mode).
      (+) The HAL_DMAEx_List_Start_IT() function allows to start the DMA channel transfer in linked-list mode
          (Non-blocking mode).
              (++) It is mandatory to register a linked-list queue to be executed by a DMA channel before starting
                   transfer otherwise a HAL_ERROR will be returned.

@endverbatim
  * @{
  */

/**
  * @brief  Start the DMA channel transfer in linked-list mode (Blocking mode).
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_Start(DMA_HandleTypeDef *const hdma)
{
  HAL_DMA_StateTypeDef dma_state;
  uint32_t ccr_value;
  uint32_t cllr_mask;

  /* Check the DMA peripheral handle and the linked-list queue parameters */
  if ((hdma == NULL) || (hdma->LinkedListQueue == NULL))
  {
    return HAL_ERROR;
  }

  /* Check DMA channel state */
  dma_state = hdma->State;
  ccr_value = hdma->Instance->CCR & DMA_CCR_LSM;
  if ((dma_state == HAL_DMA_STATE_READY) || ((dma_state == HAL_DMA_STATE_BUSY) && (ccr_value != 0U)))
  {
    /* Check DMA channel state is ready */
    if (hdma->State == HAL_DMA_STATE_READY)
    {
      /* Process locked */
      __HAL_LOCK(hdma);

      /* Update the DMA channel and the queue states */
      hdma->State                  = HAL_DMA_STATE_BUSY;
      hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_BUSY;

      /* Update the DMA channel and the queue error codes */
      hdma->ErrorCode                  = HAL_DMA_ERROR_NONE;
      hdma->LinkedListQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

      /* Get CLLR register mask and offset */
      DMA_List_GetCLLRNodeInfo(hdma->LinkedListQueue->Head, &cllr_mask, NULL);

      /* Update DMA registers for linked-list transfer */
      hdma->Instance->CLBAR = ((uint32_t)hdma->LinkedListQueue->Head & DMA_CLBAR_LBA);
      hdma->Instance->CLLR  = ((uint32_t)hdma->LinkedListQueue->Head & DMA_CLLR_LA) | cllr_mask;
    }

    /* Enable DMA channel */
    __HAL_DMA_ENABLE(hdma);
  }
  else
  {
    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_BUSY;

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Starts the DMA channel transfer in linked-list mode with interrupts enabled (Non-blocking mode).
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_Start_IT(DMA_HandleTypeDef *const hdma)
{
  HAL_DMA_StateTypeDef dma_state;
  uint32_t ccr_value;
  uint32_t cllr_mask;

  /* Check the DMA peripheral handle and the linked-list queue parameters */
  if ((hdma == NULL) || (hdma->LinkedListQueue == NULL))
  {
    return HAL_ERROR;
  }

  /* Check DMA channel state */
  dma_state = hdma->State;
  ccr_value = hdma->Instance->CCR & DMA_CCR_LSM;
  if ((dma_state == HAL_DMA_STATE_READY) || ((dma_state == HAL_DMA_STATE_BUSY) && (ccr_value != 0U)))
  {
    /* Check DMA channel state is ready */
    if (hdma->State == HAL_DMA_STATE_READY)
    {
      /* Process locked */
      __HAL_LOCK(hdma);

      /* Update the DMA channel and the queue states */
      hdma->State                  = HAL_DMA_STATE_BUSY;
      hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_BUSY;

      /* Update the DMA channel and the queue error codes */
      hdma->ErrorCode                  = HAL_DMA_ERROR_NONE;
      hdma->LinkedListQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

      /* Enable common interrupts: Transfer Complete and Transfer Errors ITs */
      __HAL_DMA_ENABLE_IT(hdma, (DMA_IT_TC | DMA_IT_DTE | DMA_IT_ULE | DMA_IT_USE | DMA_IT_TO));

      /* Check half transfer complete callback */
      if (hdma->XferHalfCpltCallback != NULL)
      {
        /* If half transfer complete callback is set, enable the corresponding IT */
        __HAL_DMA_ENABLE_IT(hdma, DMA_IT_HT);
      }

      /* Check suspend callback */
      if (hdma->XferSuspendCallback != NULL)
      {
        /* If transfer suspend callback is set, enable the corresponding IT */
        __HAL_DMA_ENABLE_IT(hdma, DMA_IT_SUSP);
      }

      /* Get CLLR register mask and offset */
      DMA_List_GetCLLRNodeInfo(hdma->LinkedListQueue->Head, &cllr_mask, NULL);

      /* Update DMA registers for linked-list transfer */
      hdma->Instance->CLBAR = ((uint32_t)hdma->LinkedListQueue->Head & DMA_CLBAR_LBA);
      hdma->Instance->CLLR  = ((uint32_t)hdma->LinkedListQueue->Head & DMA_CLLR_LA) | cllr_mask;
    }

    /* Enable DMA channel */
    __HAL_DMA_ENABLE(hdma);
  }
  else
  {
    /* Change the error code */
    hdma->ErrorCode = HAL_DMA_ERROR_BUSY;

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }

  return HAL_OK;
}
/**
  * @}
  */

/** @addtogroup DMAEx_Exported_Functions_Group3
  *
@verbatim
  ======================================================================================================================
                         ##### Linked-List Management Functions #####
  ======================================================================================================================
    [..]
      This section provides functions allowing to :
      (+) Build linked-list node.
      (+) Get linked-list node configuration.
      (+) Insert node to linked-list queue in any queue position.
      (+) Remove any node from linked-list queue.
      (+) Replace any node from linked-list queue.
      (+) Reset linked-list queue.
      (+) Insert linked-list queue in any queue position.
      (+) Set circular mode configuration to linked-list queue.
      (+) Clear circular mode configuration from linked-list queue.
      (+) Convert static linked-list queue to dynamic format.
      (+) Convert dynamic linked-list queue to static format.
      (+) Link linked-list queue to DMA channel.
      (+) Unlink linked-list queue from DMA channel.

    [..]
      (+) The HAL_DMAEx_List_BuildNode() function allows to build linked-list node.
          Node type can be :
              (++) 2 dimensions addressing node.
              (++) Linear addressing node.

      (+) The HAL_DMAEx_List_GetNodeConfig() function allows to get the linked-list node configuration from built node.

      (+) The HAL_DMAEx_List_InsertNode() function allows to insert built linked-list node to static linked-list queue
          according to selected position.

      (+) The HAL_DMAEx_List_InsertNode_Head() and HAL_DMAEx_List_InsertNode_Tail() functions allow to insert built
          linked-list node to the head (respectively the tail) of static linked-list queue.

      (+) The HAL_DMAEx_List_RemoveNode() function allows to remove selected built linked-list node from static
          linked-list queue.

      (+) The HAL_DMAEx_List_RemoveNode_Head() and HAL_DMAEx_List_RemoveNode_Tail() functions allow to remove the head
          (respectively the tail) built linked-list node from static linked-list queue.

      (+) The HAL_DMAEx_List_ReplaceNode() function allows to replace selected built linked-list node from static
          linked-list queue.

      (+) The HAL_DMAEx_List_ReplaceNode_Head() and HAL_DMAEx_List_ReplaceNode_Tail() functions allow to replace the
          head (respectively the tail) built linked-list node of static linked-list queue.

      (+) The HAL_DMAEx_List_ResetQ() function allows to reset static linked-list queue and unlink all built linked-list
          nodes.

      (+) The HAL_DMAEx_List_InsertQ() function allows to insert static linked-list source queue to static linked-list
          destination queue according to selected position.

      (+) The HAL_DMAEx_List_InsertQ_Head() and HAL_DMAEx_List_InsertQ_Tail() functions allow to insert static
          linked-list source queue to the head (respectively the tail) of static linked-list destination queue.

      (+) The HAL_DMAEx_List_SetCircularModeConfig() function allows to link the last static linked-list queue node to
          the selected first circular node.

      (+) The HAL_DMAEx_List_SetCircularMode() function allows to link the last static linked-list queue node to the
          first static linked-list queue node.

      (+) The HAL_DMAEx_List_ClearCircularMode() function allows to unlink the last static linked-list queue node from
          any first circular node position.

      (+) The HAL_DMAEx_List_ConvertQToDynamic() function allows to convert the static linked-list queue to dynamic
          format. (Optimized queue execution)

      (+) The HAL_DMAEx_List_ConvertQToStatic() function allows to convert the dynamic linked-list queue to static
          format. (Not optimized queue execution)

      (+) The HAL_DMAEx_List_LinkQ() function allows to link the (Dynamic / Static) linked-list queue to DMA channel to
          be executed.

      (+) The HAL_DMAEx_List_UnLinkQ() function allows to unlink the (Dynamic / Static) linked-list queue from DMA
          channel when execution is completed.

@endverbatim
  * @{
  */

/**
  * @brief  Build a DMA channel node according to the specified parameters in the DMA_NodeConfTypeDef.
  * @param  pNodeConfig : Pointer to a DMA_NodeConfTypeDef structure that contains the configuration information for the
  *                       specified DMA linked-list Node.
  * @param  pNode       : Pointer to a DMA_NodeTypeDef structure that contains linked-list node registers
  *                       configurations.
  * @note   The DMA linked-list node parameter address should be 32bit aligned and should not exceed the 64 KByte
  *         addressable space.
  * @note   Warning if AXI port is selected with HPDMA, the maximum source (and destination) length should be less
  *         than 17. Otherwise, an error will be returned and no initialization performed.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_BuildNode(DMA_NodeConfTypeDef const *const pNodeConfig,
                                           DMA_NodeTypeDef *const pNode)
{
  /* Check the node configuration and physical node parameters */
  if ((pNodeConfig == NULL) || (pNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check node type parameter */
  assert_param(IS_DMA_NODE_TYPE(pNodeConfig->NodeType));

  /* Check DMA channel basic transfer parameters */
  assert_param(IS_DMA_SOURCE_INC(pNodeConfig->Init.SrcInc));
  assert_param(IS_DMA_DESTINATION_INC(pNodeConfig->Init.DestInc));
  assert_param(IS_DMA_SOURCE_DATA_WIDTH(pNodeConfig->Init.SrcDataWidth));
  assert_param(IS_DMA_DESTINATION_DATA_WIDTH(pNodeConfig->Init.DestDataWidth));
  assert_param(IS_DMA_DATA_ALIGNMENT(pNodeConfig->DataHandlingConfig.DataAlignment));
  assert_param(IS_DMA_REQUEST(pNodeConfig->Init.Request));
  assert_param(IS_DMA_DIRECTION(pNodeConfig->Init.Direction));
  assert_param(IS_DMA_TCEM_EVENT_MODE(pNodeConfig->Init.TransferEventMode));
  assert_param(IS_DMA_BLOCK_HW_REQUEST(pNodeConfig->Init.BlkHWRequest));
  assert_param(IS_DMA_MODE(pNodeConfig->Init.Mode));

  /* Check DMA channel parameters */
  if ((pNodeConfig->NodeType & (DMA_CHANNEL_TYPE_GPDMA | DMA_CHANNEL_TYPE_HPDMA)) != 0U)
  {
    assert_param(IS_DMA_BURST_LENGTH(pNodeConfig->Init.SrcBurstLength));
    assert_param(IS_DMA_BURST_LENGTH(pNodeConfig->Init.DestBurstLength));
    assert_param(IS_DMA_DATA_EXCHANGE(pNodeConfig->DataHandlingConfig.DataExchange));
    assert_param(IS_DMA_TRANSFER_ALLOCATED_PORT(pNodeConfig->Init.TransferAllocatedPort));
  }

  /* Check DMA channel trigger parameters */
  assert_param(IS_DMA_TRIGGER_POLARITY(pNodeConfig->TriggerConfig.TriggerPolarity));
  if (pNodeConfig->TriggerConfig.TriggerPolarity != DMA_TRIG_POLARITY_MASKED)
  {
    assert_param(IS_DMA_TRIGGER_MODE(pNodeConfig->TriggerConfig.TriggerMode));
    assert_param(IS_DMA_TRIGGER_SELECTION(pNodeConfig->TriggerConfig.TriggerSelection));
  }

  /* Check DMA channel repeated block parameters */
  if ((pNodeConfig->NodeType & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
  {
    assert_param(IS_DMA_REPEAT_COUNT(pNodeConfig->RepeatBlockConfig.RepeatCount));
    assert_param(IS_DMA_BURST_ADDR_OFFSET(pNodeConfig->RepeatBlockConfig.SrcAddrOffset));
    assert_param(IS_DMA_BURST_ADDR_OFFSET(pNodeConfig->RepeatBlockConfig.DestAddrOffset));
    assert_param(IS_DMA_BLOCK_ADDR_OFFSET(pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset));
    assert_param(IS_DMA_BLOCK_ADDR_OFFSET(pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset));
  }

  /* Check DMA channel security and privilege attributes parameters */
#if defined (CPU_IN_SECURE_STATE)
  assert_param(IS_DMA_ATTRIBUTES(pNodeConfig->SrcSecure));
  assert_param(IS_DMA_ATTRIBUTES(pNodeConfig->DestSecure));
#endif /* CPU_IN_SECURE_STATE */

  if ((pNodeConfig->NodeType & DMA_CHANNEL_TYPE_HPDMA) == DMA_CHANNEL_TYPE_HPDMA)
  {
    if (((pNodeConfig->Init.TransferAllocatedPort & DMA_CTR1_SAP) == DMA_SRC_ALLOCATED_PORT0) &&
        (pNodeConfig->Init.SrcBurstLength > 16U))
    {
      return HAL_ERROR;
    }
    if (((pNodeConfig->Init.TransferAllocatedPort & DMA_CTR1_DAP) == DMA_DEST_ALLOCATED_PORT0) &&
        (pNodeConfig->Init.DestBurstLength > 16U))
    {
      return HAL_ERROR;
    }
  }

  /* Build the DMA channel node */
  DMA_List_BuildNode(pNodeConfig, pNode);

  return HAL_OK;
}

/**
  * @brief  Get a DMA channel node configuration.
  * @param  pNodeConfig : Pointer to a DMA_NodeConfTypeDef structure that contains the configuration information for the
  *                       specified DMA linked-list Node.
  * @param  pNode       : Pointer to a DMA_NodeTypeDef structure that contains linked-list node registers
  *                       configurations.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_GetNodeConfig(DMA_NodeConfTypeDef *const pNodeConfig,
                                               DMA_NodeTypeDef const *const pNode)
{
  /* Check the node configuration and physical node parameters */
  if ((pNodeConfig == NULL) || (pNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Get the DMA channel node configuration */
  DMA_List_GetNodeConfig(pNodeConfig, pNode);

  return HAL_OK;
}

/**
  * @brief  Insert new node in any queue position of linked-list queue according to selecting previous node.
  * @param  pQList    : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @param  pPrevNode : Pointer to a DMA_NodeTypeDef structure that contains linked-list previous node registers
  *                     configurations.
  * @param  pNewNode  : Pointer to a DMA_NodeTypeDef structure that contains linked-list new node registers
  *                     configurations.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_InsertNode(DMA_QListTypeDef *const pQList,
                                            DMA_NodeTypeDef *const pPrevNode,
                                            DMA_NodeTypeDef *const pNewNode)
{
  uint32_t cllr_mask;
  uint32_t cllr_offset;
  DMA_NodeInQInfoTypeDef node_info;

  /* Check the queue and the new node parameters */
  if ((pQList == NULL) || (pNewNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check queue type */
  if (pQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Check nodes base addresses */
  if (DMA_List_CheckNodesBaseAddresses(pQList->Head, pPrevNode, pNewNode) != 0U)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;

    return HAL_ERROR;
  }

  /* Check nodes types compatibility */
  if (DMA_List_CheckNodesTypes(pQList->Head, pPrevNode, pNewNode) != 0U)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Get CLLR register mask and offset */
  DMA_List_GetCLLRNodeInfo(pNewNode, &cllr_mask, &cllr_offset);

  /* Empty queue */
  if (pQList->Head == NULL)
  {
    /* Add only new node to queue */
    if (pPrevNode == NULL)
    {
      pQList->Head       = pNewNode;
      pQList->NodeNumber = 1U;
    }
    /* Add previous node then new node to queue */
    else
    {
      pQList->Head                          = pPrevNode;
      pPrevNode->LinkRegisters[cllr_offset] = ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;
      pQList->NodeNumber                    = 2U;
    }
  }
  /* Not empty queue */
  else
  {
    /* Add new node at the head of queue */
    if (pPrevNode == NULL)
    {
      pNewNode->LinkRegisters[cllr_offset] = ((uint32_t)pQList->Head & DMA_CLLR_LA) | cllr_mask;
      pQList->Head                         = pNewNode;
    }
    /* Add new node according to selected position */
    else
    {
      /* Find node and get its position in selected queue */
      node_info.cllr_offset = cllr_offset;
      if (DMA_List_FindNode(pQList, pPrevNode, &node_info) == 0U)
      {
        /* Selected node is the last queue node */
        if (node_info.currentnode_pos == pQList->NodeNumber)
        {
          /* Check if queue is circular */
          if (pQList->FirstCircularNode != NULL)
          {
            pNewNode->LinkRegisters[cllr_offset] = ((uint32_t)pQList->FirstCircularNode & DMA_CLLR_LA) | cllr_mask;
          }

          pPrevNode->LinkRegisters[cllr_offset] = ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;
        }
        /* Selected node is not the last queue node */
        else
        {
          pNewNode->LinkRegisters[cllr_offset] = pPrevNode->LinkRegisters[cllr_offset];
          pPrevNode->LinkRegisters[cllr_offset] = ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;
        }
      }
      else
      {
        /* Update the queue error code */
        pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NOTFOUND;

        return HAL_ERROR;
      }
    }

    /* Increment queue node number */
    pQList->NodeNumber++;
  }

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Insert new node at the head of linked-list queue.
  * @param  pQList    : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @param  pNewNode  : Pointer to a DMA_NodeTypeDef structure that contains linked-list new node registers
  *                     configurations.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_InsertNode_Head(DMA_QListTypeDef *const pQList,
                                                 DMA_NodeTypeDef *const pNewNode)
{
  uint32_t cllr_mask;
  uint32_t cllr_offset;

  /* Check the queue and the new node parameters */
  if ((pQList == NULL) || (pNewNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check queue type */
  if (pQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Check nodes base addresses */
  if (DMA_List_CheckNodesBaseAddresses(pQList->Head, pNewNode, NULL) != 0U)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;

    return HAL_ERROR;
  }

  /* Check nodes types compatibility */
  if (DMA_List_CheckNodesTypes(pQList->Head, pNewNode, NULL) != 0U)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Empty queue */
  if (pQList->Head == NULL)
  {
    pQList->Head = pNewNode;
  }
  /* Not empty queue */
  else
  {
    /* Get CLLR register mask and offset */
    DMA_List_GetCLLRNodeInfo(pNewNode, &cllr_mask, &cllr_offset);

    pNewNode->LinkRegisters[cllr_offset] = ((uint32_t)pQList->Head & DMA_CLLR_LA) | cllr_mask;
    pQList->Head                         = pNewNode;
  }

  /* Increment queue node number */
  pQList->NodeNumber++;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Insert new node at the tail of linked-list queue.
  * @param  pQList    : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @param  pNewNode  : Pointer to a DMA_NodeTypeDef structure that contains linked-list new node registers
  *                     configurations.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_InsertNode_Tail(DMA_QListTypeDef *const pQList,
                                                 DMA_NodeTypeDef *const pNewNode)
{
  uint32_t cllr_mask;
  uint32_t cllr_offset;
  DMA_NodeInQInfoTypeDef node_info;

  /* Check the queue and the new node parameters */
  if ((pQList == NULL) || (pNewNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check queue type */
  if (pQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Check nodes base addresses */
  if (DMA_List_CheckNodesBaseAddresses(pQList->Head, pNewNode, NULL) != 0U)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;

    return HAL_ERROR;
  }

  /* Check nodes types compatibility */
  if (DMA_List_CheckNodesTypes(pQList->Head, pNewNode, NULL) != 0U)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Empty queue */
  if (pQList->Head == NULL)
  {
    pQList->Head = pNewNode;
  }
  /* Not empty queue */
  else
  {
    /* Get CLLR register mask and offset */
    DMA_List_GetCLLRNodeInfo(pNewNode, &cllr_mask, &cllr_offset);

    /* Find node and get its position in selected queue */
    node_info.cllr_offset = cllr_offset;
    (void)DMA_List_FindNode(pQList, NULL, &node_info);

    /* Check if queue is circular */
    if (pQList->FirstCircularNode != NULL)
    {
      pNewNode->LinkRegisters[cllr_offset] = ((uint32_t)pQList->FirstCircularNode & DMA_CLLR_LA) | cllr_mask;
    }

    ((DMA_NodeTypeDef *)node_info.currentnode_addr)->LinkRegisters[cllr_offset] =
      ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;
  }

  /* Increment queue node number */
  pQList->NodeNumber++;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_READY;

  /* Prevent MISRA-C2012-Rule-2.2_b */
  UNUSED(node_info);

  return HAL_OK;
}

/**
  * @brief  Remove node from any linked-list queue position.
  * @param  pQList : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @param  pNode  : Pointer to a DMA_NodeTypeDef structure that contains linked-list previous node registers
  *                  configurations.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_RemoveNode(DMA_QListTypeDef *const pQList,
                                            DMA_NodeTypeDef *const pNode)
{
  uint32_t previousnode_addr;
  uint32_t cllr_offset;
  DMA_NodeInQInfoTypeDef node_info;

  /* Check the queue and the node parameters */
  if ((pQList == NULL) || (pNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the queue */
  if (pQList->Head == NULL)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;

    return HAL_ERROR;
  }

  /* Check queue type */
  if (pQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Get CLLR register mask and offset */
  DMA_List_GetCLLRNodeInfo(pNode, NULL, &cllr_offset);

  /* Find node and get its position in selected queue */
  node_info.cllr_offset = cllr_offset;
  if (DMA_List_FindNode(pQList, pNode, &node_info) == 0U)
  {
    /* Removed node is the head node */
    if (node_info.currentnode_pos == 1U)
    {
      /* Check if first circular node queue is the first node */
      if (pQList->FirstCircularNode == ((DMA_NodeTypeDef *)node_info.currentnode_addr))
      {
        /* Find last queue node */
        (void)DMA_List_FindNode(pQList, NULL, &node_info);

        /* Clear last node link */
        ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset] = 0U;

        /* Clear first circular node */
        pQList->FirstCircularNode = NULL;
      }

      /* Update the queue head node */
      pQList->Head = (DMA_NodeTypeDef *)(((uint32_t)pQList->Head & DMA_CLBAR_LBA) +
                                         (pNode->LinkRegisters[cllr_offset] & DMA_CLLR_LA));
      /* Unlink node to be removed */
      pNode->LinkRegisters[cllr_offset] = 0U;
    }
    /* Removed node is the last node */
    else if (node_info.currentnode_pos == pQList->NodeNumber)
    {
      /* Clear CLLR for previous node */
      ((DMA_NodeTypeDef *)(node_info.previousnode_addr))->LinkRegisters[cllr_offset] = 0U;

      /* Clear CLLR for last node */
      ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset] = 0U;

      /* Clear first circular node */
      pQList->FirstCircularNode = NULL;
    }
    /* Removed node is in the middle */
    else
    {
      /* Store previous node address to be updated later */
      previousnode_addr = node_info.previousnode_addr;

      /* Check if first circular node queue is the current node */
      if (pQList->FirstCircularNode == ((DMA_NodeTypeDef *)node_info.currentnode_addr))
      {
        /* Find last queue node */
        (void)DMA_List_FindNode(pQList, NULL, &node_info);

        /* Clear last node link */
        ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset] = 0U;

        /* Clear first circular node */
        pQList->FirstCircularNode = NULL;
      }

      /* Link previous node */
      ((DMA_NodeTypeDef *)(previousnode_addr))->LinkRegisters[cllr_offset] = pNode->LinkRegisters[cllr_offset];

      /* Unlink node to be removed */
      pNode->LinkRegisters[cllr_offset] = 0U;
    }

    /* Decrement node number */
    pQList->NodeNumber--;
  }
  else
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NOTFOUND;

    return HAL_ERROR;
  }

  /* Check if queue is empty */
  if (pQList->NodeNumber == 0U)
  {
    /* Clean empty queue parameter */
    DMA_List_CleanQueue(pQList);
  }
  else
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

    /* Update the queue state */
    pQList->State = HAL_DMA_QUEUE_STATE_READY;
  }

  /* Prevent MISRA-C2012-Rule-2.2_b */
  UNUSED(node_info);

  return HAL_OK;
}

/**
  * @brief  Remove the head node from linked-list queue.
  * @param  pQList : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_RemoveNode_Head(DMA_QListTypeDef *const pQList)
{
  uint32_t cllr_offset;
  uint32_t current_addr;
  DMA_NodeInQInfoTypeDef node_info;

  /* Check the queue parameter */
  if (pQList == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the queue */
  if (pQList->Head == NULL)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;

    return HAL_ERROR;
  }

  /* Check queue type */
  if (pQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Get CLLR register mask and offset */
  DMA_List_GetCLLRNodeInfo(pQList->Head, NULL, &cllr_offset);

  /* Queue contains only one node */
  if (pQList->NodeNumber == 1U)
  {
    pQList->Head->LinkRegisters[cllr_offset] = 0U;
    pQList->FirstCircularNode = 0U;
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  }
  /* Queue contains more then one node */
  else
  {
    /* Check if first circular node queue is the first node */
    if (pQList->FirstCircularNode == pQList->Head)
    {
      /* Find last queue node */
      node_info.cllr_offset = cllr_offset;
      (void)DMA_List_FindNode(pQList, NULL, &node_info);

      /* Clear last node link */
      ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset] = 0U;

      /* Clear first circular node */
      pQList->FirstCircularNode = NULL;
    }

    current_addr = pQList->Head->LinkRegisters[cllr_offset] & DMA_CLLR_LA;
    pQList->Head->LinkRegisters[cllr_offset] = 0U;
    pQList->Head = ((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQList->Head & DMA_CLBAR_LBA)));
  }

  /* Decrement node number */
  pQList->NodeNumber--;

  /* Check if queue is empty */
  if (pQList->NodeNumber == 0U)
  {
    /* Clean empty queue parameter */
    DMA_List_CleanQueue(pQList);
  }
  else
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

    /* Update the queue state */
    pQList->State = HAL_DMA_QUEUE_STATE_READY;
  }

  /* Prevent MISRA-C2012-Rule-2.2_b */
  UNUSED(node_info);

  return HAL_OK;
}

/**
  * @brief  Remove the tail node from linked-list queue.
  * @param  pQList : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_RemoveNode_Tail(DMA_QListTypeDef *const pQList)
{
  uint32_t cllr_offset;
  DMA_NodeInQInfoTypeDef node_info;

  /* Check the queue parameter */
  if (pQList == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the queue */
  if (pQList->Head == NULL)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;

    return HAL_ERROR;
  }

  /* Check queue type */
  if (pQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Get CLLR register mask and offset */
  DMA_List_GetCLLRNodeInfo(pQList->Head, NULL, &cllr_offset);

  /* Queue contains only one node */
  if (pQList->NodeNumber == 1U)
  {
    pQList->Head->LinkRegisters[cllr_offset] = 0U;
    pQList->FirstCircularNode = 0U;
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  }
  /* Queue contains more then one node */
  else
  {
    /* Find node and get its position in selected queue */
    node_info.cllr_offset = cllr_offset;
    (void)DMA_List_FindNode(pQList, NULL, &node_info);

    /* Clear CLLR for previous node */
    ((DMA_NodeTypeDef *)(node_info.previousnode_addr))->LinkRegisters[cllr_offset] = 0U;

    /* Clear CLLR for last node */
    ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset] = 0U;

    /* Clear first circular node */
    pQList->FirstCircularNode = NULL;
  }

  /* Decrement node number */
  pQList->NodeNumber--;

  /* Check if queue is empty */
  if (pQList->NodeNumber == 0U)
  {
    /* Clean empty queue parameter */
    DMA_List_CleanQueue(pQList);
  }
  else
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

    /* Update the queue state */
    pQList->State = HAL_DMA_QUEUE_STATE_READY;
  }

  /* Prevent MISRA-C2012-Rule-2.2_b */
  UNUSED(node_info);

  return HAL_OK;
}

/**
  * @brief  Replace node in linked-list queue.
  * @param  pQList   : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @param  pOldNode : Pointer to a DMA_NodeTypeDef structure that contains linked-list old node registers
  *                    configurations.
  * @param  pNewNode : Pointer to a DMA_NodeTypeDef structure that contains linked-list new node registers
  *                    configurations.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_ReplaceNode(DMA_QListTypeDef *const pQList,
                                             DMA_NodeTypeDef *const pOldNode,
                                             DMA_NodeTypeDef *const pNewNode)
{
  uint32_t cllr_mask;
  uint32_t cllr_offset;
  DMA_NodeInQInfoTypeDef node_info;

  /* Check the queue and the nodes parameters */
  if ((pQList == NULL) || (pOldNode == NULL) || (pNewNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the queue */
  if (pQList->Head == NULL)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;

    return HAL_ERROR;
  }

  /* Check queue type */
  if (pQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Check nodes base addresses */
  if (DMA_List_CheckNodesBaseAddresses(pQList->Head, pOldNode, pNewNode) != 0U)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;

    return HAL_ERROR;
  }

  /* Check nodes types compatibility */
  if (DMA_List_CheckNodesTypes(pQList->Head, pOldNode, pNewNode) != 0U)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Get CLLR register mask and offset */
  DMA_List_GetCLLRNodeInfo(pNewNode, &cllr_mask, &cllr_offset);

  /* Find node and get its position in selected queue */
  node_info.cllr_offset = cllr_offset;
  if (DMA_List_FindNode(pQList, pOldNode, &node_info) == 0U)
  {
    /* Replaced node is the head node */
    if (node_info.currentnode_pos == 1U)
    {
      pNewNode->LinkRegisters[cllr_offset] =
        ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset];
      pQList->Head = pNewNode;
      ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset] = 0U;

      /* Check if first circular node queue is the first node */
      if (pQList->FirstCircularNode == ((DMA_NodeTypeDef *)node_info.currentnode_addr))
      {
        /* Find last queue node */
        (void)DMA_List_FindNode(pQList, NULL, &node_info);

        /* Clear last node link */
        ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset] =
          ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;

        /* Set new node as first circular node */
        pQList->FirstCircularNode = pNewNode;
      }
    }
    /* Replaced node is the last */
    else if (node_info.currentnode_pos == pQList->NodeNumber)
    {
      ((DMA_NodeTypeDef *)(node_info.previousnode_addr))->LinkRegisters[cllr_offset] =
        ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;
      ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset] = 0U;

      /* Check if first circular node queue is the last node */
      if (pQList->FirstCircularNode == ((DMA_NodeTypeDef *)(node_info.currentnode_addr)))
      {
        /* Link first circular node to new node */
        pNewNode->LinkRegisters[cllr_offset] = ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;

        /* Set new node as first circular node */
        pQList->FirstCircularNode = pNewNode;
      }
      /* Check if first circular node queue is not the last node */
      else if (pQList->FirstCircularNode != NULL)
      {
        /* Link first circular node to new node */
        pNewNode->LinkRegisters[cllr_offset] = ((uint32_t)pQList->FirstCircularNode & DMA_CLLR_LA) | cllr_mask;
      }
      else
      {
        /* Prevent MISRA-C2012-Rule-15.7 */
      }
    }
    /* Replaced node is in the middle */
    else
    {
      ((DMA_NodeTypeDef *)(node_info.previousnode_addr))->LinkRegisters[cllr_offset] =
        ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;
      pNewNode->LinkRegisters[cllr_offset] =
        ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset];
      ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset] = 0U;

      /* Check if first circular node queue is the current node */
      if (pQList->FirstCircularNode == ((DMA_NodeTypeDef *)(node_info.currentnode_addr)))
      {
        /* Find last node and get its position in selected queue */
        (void)DMA_List_FindNode(pQList, NULL, &node_info);

        /* Link last queue node to new node */
        ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset] =
          ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;

        /* Set new node as first circular node */
        pQList->FirstCircularNode = pNewNode;
      }
    }
  }
  else
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NOTFOUND;

    return HAL_ERROR;
  }

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_READY;

  /* Prevent MISRA-C2012-Rule-2.2_b */
  UNUSED(node_info);

  return HAL_OK;
}

/**
  * @brief  Replace the head node of linked-list queue.
  * @param  pQList   : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @param  pNewNode : Pointer to a DMA_NodeTypeDef structure that contains linked-list new node registers
  *                    configurations.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_ReplaceNode_Head(DMA_QListTypeDef *const pQList,
                                                  DMA_NodeTypeDef *const pNewNode)
{
  uint32_t cllr_offset;
  uint32_t cllr_mask;
  DMA_NodeInQInfoTypeDef node_info;

  /* Check the queue and the new node parameters */
  if ((pQList == NULL) || (pNewNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the queue */
  if (pQList->Head == NULL)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;

    return HAL_ERROR;
  }

  /* Check queue type */
  if (pQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Check nodes base addresses */
  if (DMA_List_CheckNodesBaseAddresses(pQList->Head, pNewNode, NULL) != 0U)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;

    return HAL_ERROR;
  }

  /* Check nodes types compatibility */
  if (DMA_List_CheckNodesTypes(pQList->Head, pNewNode, NULL) != 0U)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Get CLLR register mask and offset */
  DMA_List_GetCLLRNodeInfo(pNewNode, &cllr_mask, &cllr_offset);

  /* Check if first circular node queue is the first node */
  if (pQList->FirstCircularNode == pQList->Head)
  {
    /* Find last queue node */
    node_info.cllr_offset = cllr_offset;
    (void)DMA_List_FindNode(pQList, NULL, &node_info);

    /* Clear last node link */
    ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset] =
      ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;

    /* Set new node as first circular node */
    pQList->FirstCircularNode = pNewNode;
  }

  /* Replace head node */
  pNewNode->LinkRegisters[cllr_offset] = pQList->Head->LinkRegisters[cllr_offset];
  pQList->Head->LinkRegisters[cllr_offset] = 0U;
  pQList->Head = pNewNode;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_READY;

  /* Prevent MISRA-C2012-Rule-2.2_b */
  UNUSED(node_info);

  return HAL_OK;
}

/**
  * @brief  Replace the tail node of linked-list queue.
  * @param  pQList   : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @param  pNewNode : Pointer to a DMA_NodeTypeDef structure that contains linked-list new node registers
  *                    configurations.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_ReplaceNode_Tail(DMA_QListTypeDef *const pQList,
                                                  DMA_NodeTypeDef *const pNewNode)
{
  uint32_t cllr_mask;
  uint32_t cllr_offset;
  DMA_NodeInQInfoTypeDef node_info;

  /* Check the queue and the new node parameters */
  if ((pQList == NULL) || (pNewNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the queue */
  if (pQList->Head == NULL)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;

    return HAL_ERROR;
  }

  /* Check queue type */
  if (pQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Get CLLR register mask and offset */
  DMA_List_GetCLLRNodeInfo(pNewNode, &cllr_mask, &cllr_offset);

  /* Find last node and get its position in selected queue */
  node_info.cllr_offset = cllr_offset;
  (void)DMA_List_FindNode(pQList, NULL, &node_info);

  /* Link previous node to new node */
  ((DMA_NodeTypeDef *)(node_info.previousnode_addr))->LinkRegisters[cllr_offset] =
    ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;

  /* Clear CLLR for current node */
  ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset] = 0U;

  /* Check if first circular node queue is the last node */
  if (pQList->FirstCircularNode == ((DMA_NodeTypeDef *)(node_info.currentnode_addr)))
  {
    /* Link first circular node to new node */
    pNewNode->LinkRegisters[cllr_offset] = ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;

    /* Set new node as first circular node */
    pQList->FirstCircularNode = pNewNode;
  }
  /* Check if first circular node queue is not the last node */
  else if (pQList->FirstCircularNode != NULL)
  {
    /* Link first circular node to new node */
    pNewNode->LinkRegisters[cllr_offset] = ((uint32_t)pQList->FirstCircularNode & DMA_CLLR_LA) | cllr_mask;
  }
  else
  {
    /* Prevent MISRA-C2012-Rule-15.7 */
  }

  /* Check if queue contains one node */
  if (pQList->NodeNumber == 1U)
  {
    pQList->Head = pNewNode;
  }

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Reset the linked-list queue and unlink queue nodes.
  * @param  pQList : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_ResetQ(DMA_QListTypeDef *const pQList)
{
  uint32_t cllr_offset;
  DMA_NodeInQInfoTypeDef node_info;

  /* Check the queue parameter */
  if (pQList == NULL)
  {
    return HAL_ERROR;
  }

  /* Check queue state */
  if (pQList->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_BUSY;

    return HAL_ERROR;
  }

  /* Check queue type */
  if (pQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Check the queue */
  if (pQList->Head != NULL)
  {
    /* Get CLLR register mask and offset */
    DMA_List_GetCLLRNodeInfo(pQList->Head, NULL, &cllr_offset);

    /* Reset selected queue nodes */
    node_info.cllr_offset = cllr_offset;
    DMA_List_ResetQueueNodes(pQList, &node_info);
  }

  /* Reset head node address */
  pQList->Head = NULL;

  /* Reset node number */
  pQList->NodeNumber = 0U;

  /* Reset first circular node */
  pQList->FirstCircularNode = NULL;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_RESET;

  return HAL_OK;
}

/**
  * @brief  Insert a source linked-list queue to a destination linked-list queue according to selecting previous node.
  * @param  pSrcQList  : Pointer to a DMA_QListTypeDef structure that contains source queue information.
  * @param  pPrevNode  : Pointer to a DMA_NodeTypeDef structure that contains linked-list previous node registers
  *                      configurations.
  * @param  pDestQList : Pointer to a DMA_QListTypeDef structure that contains destination queue information.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_InsertQ(DMA_QListTypeDef *const pSrcQList,
                                         DMA_NodeTypeDef const *const pPrevNode,
                                         DMA_QListTypeDef *const pDestQList)
{
  uint32_t cllr_mask;
  uint32_t cllr_offset;
  DMA_NodeInQInfoTypeDef src_q_node_info;
  DMA_NodeInQInfoTypeDef dest_q_node_info;

  /* Check the source and destination queues and the previous node parameters */
  if ((pSrcQList == NULL) || (pDestQList == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the source queue */
  if (pSrcQList->Head == NULL)
  {
    /* Update the queue error code */
    pSrcQList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;

    return HAL_ERROR;
  }

  /* Check the source queue type */
  if (pSrcQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pSrcQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Check the destination queue type */
  if (pDestQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pDestQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Check the source queue circularity */
  if (pSrcQList->FirstCircularNode != NULL)
  {
    /* Update the source queue error code */
    pSrcQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Check nodes base addresses */
  if (DMA_List_CheckNodesBaseAddresses(pSrcQList->Head, pPrevNode, pDestQList->Head) != 0U)
  {
    /* Update the source queue error code */
    pSrcQList->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;

    /* Update the destination queue error code */
    pDestQList->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;

    return HAL_ERROR;
  }

  /* Check nodes types compatibility */
  if (DMA_List_CheckNodesTypes(pSrcQList->Head, pPrevNode, pDestQList->Head) != 0U)
  {
    /* Update the source queue error code */
    pSrcQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    /* Update the destination queue error code */
    pDestQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Update the source queue state */
  pSrcQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the source queue error code */
  pSrcQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the destination queue state */
  pDestQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the destination queue error code */
  pDestQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Get CLLR register mask and offset */
  DMA_List_GetCLLRNodeInfo(pSrcQList->Head, &cllr_mask, &cllr_offset);

  /* Empty destination queue */
  if (pDestQList->Head == NULL)
  {
    pDestQList->Head       = pSrcQList->Head;
    pDestQList->NodeNumber = pSrcQList->NodeNumber;
  }
  /* Not empty destination queue */
  else
  {
    /* Previous node is empty */
    if (pPrevNode == NULL)
    {
      /* Find node and get its position in selected queue */
      src_q_node_info.cllr_offset = cllr_offset;
      (void)DMA_List_FindNode(pSrcQList, NULL, &src_q_node_info);

      /* Check if first circular node queue is the first node */
      if (pDestQList->FirstCircularNode == pDestQList->Head)
      {
        /* Find node and get its position in selected queue */
        dest_q_node_info.cllr_offset = cllr_offset;
        (void)DMA_List_FindNode(pDestQList, NULL, &dest_q_node_info);

        /* Link destination queue tail node to new first circular node */
        ((DMA_NodeTypeDef *)dest_q_node_info.currentnode_addr)->LinkRegisters[cllr_offset] =
          ((uint32_t)pSrcQList->Head & DMA_CLLR_LA) | cllr_mask;

        /* Set the head node of source queue as the first circular node */
        pDestQList->FirstCircularNode = pSrcQList->Head;
      }

      /* Link the last node of source queue to the fist node of destination queue */
      ((DMA_NodeTypeDef *)(src_q_node_info.currentnode_addr))->LinkRegisters[cllr_offset] =
        ((uint32_t)pDestQList->Head & DMA_CLLR_LA) | cllr_mask;
      pDestQList->Head        = pSrcQList->Head;
      pDestQList->NodeNumber += pSrcQList->NodeNumber;
    }
    /* Previous node is not empty */
    else
    {
      /* Find node and get its position in selected queue */
      dest_q_node_info.cllr_offset = cllr_offset;
      if (DMA_List_FindNode(pDestQList, pPrevNode, &dest_q_node_info) == 0U)
      {
        /* Selected node is the last destination queue node */
        if (dest_q_node_info.currentnode_pos == pDestQList->NodeNumber)
        {
          /* Link the first node of source queue to the last node of destination queue */
          ((DMA_NodeTypeDef *)(dest_q_node_info.currentnode_addr))->LinkRegisters[cllr_offset] =
            ((uint32_t)pSrcQList->Head & DMA_CLLR_LA) | cllr_mask;
          pDestQList->NodeNumber += pSrcQList->NodeNumber;

          /* Check if first circular node queue is not empty */
          if (pDestQList->FirstCircularNode != NULL)
          {
            /* Find node and get its position in selected queue */
            src_q_node_info.cllr_offset = cllr_offset;
            (void)DMA_List_FindNode(pSrcQList, NULL, &src_q_node_info);

            /* Find first circular node */
            (void)DMA_List_FindNode(pDestQList, pDestQList->FirstCircularNode, &dest_q_node_info);

            /* Link last source queue node to first destination queue */
            ((DMA_NodeTypeDef *)src_q_node_info.currentnode_addr)->LinkRegisters[cllr_offset] =
              (dest_q_node_info.currentnode_addr & DMA_CLLR_LA) | cllr_mask;
          }
        }
        /* Selected node is not the last destination queue node */
        else
        {
          /* Link the first node of source queue to the previous node of destination queue */
          ((DMA_NodeTypeDef *)(dest_q_node_info.currentnode_addr))->LinkRegisters[cllr_offset] =
            ((uint32_t)pSrcQList->Head & DMA_CLLR_LA) | cllr_mask;

          /* Find node and get its position in selected queue */
          src_q_node_info.cllr_offset = cllr_offset;
          (void)DMA_List_FindNode(pSrcQList, NULL, &src_q_node_info);

          /* Link the last node of source queue to the next node of destination queue */
          ((DMA_NodeTypeDef *)(src_q_node_info.currentnode_addr))->LinkRegisters[cllr_offset] =
            (dest_q_node_info.nextnode_addr & DMA_CLLR_LA) | cllr_mask;

          /* Update queues counter */
          pDestQList->NodeNumber += pSrcQList->NodeNumber;
        }
      }
      else
      {
        /* Update the destination queue error code */
        pDestQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NOTFOUND;

        return HAL_ERROR;
      }
    }
  }

  /* Clean the source queue variable as it is obsolete */
  DMA_List_CleanQueue(pSrcQList);

  /* Update the destination queue error code */
  pDestQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the destination queue state */
  pDestQList->State = HAL_DMA_QUEUE_STATE_READY;

  /* Prevent MISRA-C2012-Rule-2.2_b */
  UNUSED(src_q_node_info);
  UNUSED(dest_q_node_info);

  return HAL_OK;
}

/**
  * @brief  Insert a source linked-list queue at the head of destination queue.
  * @param  pSrcQList  : Pointer to a DMA_QListTypeDef structure that contains source queue information.
  * @param  pDestQList : Pointer to a DMA_QListTypeDef structure that contains destination queue information.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_InsertQ_Head(DMA_QListTypeDef *const pSrcQList,
                                              DMA_QListTypeDef *const pDestQList)
{
  uint32_t cllr_mask;
  uint32_t cllr_offset;
  DMA_NodeInQInfoTypeDef src_q_node_info;
  DMA_NodeInQInfoTypeDef dest_q_node_info;

  /* Check the source and destination queues and the previous node parameters */
  if ((pSrcQList == NULL) || (pDestQList == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the source queue */
  if (pSrcQList->Head == NULL)
  {
    /* Update the queue error code */
    pSrcQList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;

    return HAL_ERROR;
  }

  /* Check the source queue type */
  if (pSrcQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pSrcQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Check the destination queue type */
  if (pDestQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pDestQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Check nodes base addresses */
  if (DMA_List_CheckNodesBaseAddresses(pSrcQList->Head, pDestQList->Head, NULL) != 0U)
  {
    /* Update the source queue error code */
    pSrcQList->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;

    /* Update the destination queue error code */
    pDestQList->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;

    return HAL_ERROR;
  }

  /* Check nodes types compatibility */
  if (DMA_List_CheckNodesTypes(pSrcQList->Head, pDestQList->Head, NULL) != 0U)
  {
    /* Update the source queue error code */
    pSrcQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    /* Update the destination queue error code */
    pDestQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Update the source queue state */
  pSrcQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the source queue error code */
  pSrcQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the destination queue state */
  pDestQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the destination queue error code */
  pDestQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Get CLLR register mask and offset */
  DMA_List_GetCLLRNodeInfo(pSrcQList->Head, &cllr_mask, &cllr_offset);

  /* Empty destination queue */
  if (pDestQList->Head == NULL)
  {
    pDestQList->Head       = pSrcQList->Head;
    pDestQList->NodeNumber = pSrcQList->NodeNumber;
  }
  /* Not empty destination queue */
  else
  {
    /* Find node and get its position in selected queue */
    src_q_node_info.cllr_offset = cllr_offset;
    (void)DMA_List_FindNode(pSrcQList, NULL, &src_q_node_info);

    /* Check if first circular node queue is the first node */
    if (pDestQList->FirstCircularNode == pDestQList->Head)
    {
      /* Find node and get its position in selected queue */
      dest_q_node_info.cllr_offset = cllr_offset;
      (void)DMA_List_FindNode(pDestQList, NULL, &dest_q_node_info);

      /* Link destination queue tail node to new first circular node */
      ((DMA_NodeTypeDef *)dest_q_node_info.currentnode_addr)->LinkRegisters[cllr_offset] =
        ((uint32_t)pSrcQList->Head & DMA_CLLR_LA) | cllr_mask;

      /* Set the head node of source queue as the first circular node */
      pDestQList->FirstCircularNode = pSrcQList->Head;
    }

    /* Link the last node of source queue to the fist node of destination queue */
    ((DMA_NodeTypeDef *)(src_q_node_info.currentnode_addr))->LinkRegisters[cllr_offset] =
      ((uint32_t)pDestQList->Head & DMA_CLLR_LA) | cllr_mask;
    pDestQList->Head        = pSrcQList->Head;
    pDestQList->NodeNumber += pSrcQList->NodeNumber;
  }

  /* Clean the source queue variable as it is obsolete */
  DMA_List_CleanQueue(pSrcQList);

  /* Update the destination queue error code */
  pDestQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the destination queue state */
  pDestQList->State = HAL_DMA_QUEUE_STATE_READY;

  /* Prevent MISRA-C2012-Rule-2.2_b */
  UNUSED(src_q_node_info);
  UNUSED(dest_q_node_info);

  return HAL_OK;
}

/**
  * @brief  Insert a source linked-list queue at the tail of destination queue.
  * @param  pSrcQList  : Pointer to a DMA_QListTypeDef structure that contains source queue information.
  * @param  pDestQList : Pointer to a DMA_QListTypeDef structure that contains destination queue information.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_InsertQ_Tail(DMA_QListTypeDef *const pSrcQList,
                                              DMA_QListTypeDef *const pDestQList)
{
  uint32_t cllr_mask;
  uint32_t cllr_offset;
  DMA_NodeInQInfoTypeDef src_q_node_info;
  DMA_NodeInQInfoTypeDef dest_q_node_info;

  /* Check the source and destination queues and the previous node parameters */
  if ((pSrcQList == NULL) || (pDestQList == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the source queue */
  if (pSrcQList->Head == NULL)
  {
    /* Update the queue error code */
    pSrcQList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;

    return HAL_ERROR;
  }

  /* Check the source queue type */
  if (pSrcQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pSrcQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Check the destination queue type */
  if (pDestQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pDestQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Check nodes base addresses */
  if (DMA_List_CheckNodesBaseAddresses(pSrcQList->Head, pDestQList->Head, NULL) != 0U)
  {
    /* Update the source queue error code */
    pSrcQList->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;

    /* Update the destination queue error code */
    pDestQList->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;

    return HAL_ERROR;
  }

  /* Check nodes types compatibility */
  if (DMA_List_CheckNodesTypes(pSrcQList->Head, pDestQList->Head, NULL) != 0U)
  {
    /* Update the source queue error code */
    pSrcQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    /* Update the destination queue error code */
    pDestQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Update the source queue state */
  pSrcQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the source queue error code */
  pSrcQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the destination queue state */
  pDestQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the destination queue error code */
  pDestQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Get CLLR register mask and offset */
  DMA_List_GetCLLRNodeInfo(pSrcQList->Head, &cllr_mask, &cllr_offset);

  /* Empty destination queue */
  if (pDestQList->Head == NULL)
  {
    pDestQList->Head       = pSrcQList->Head;
    pDestQList->NodeNumber = pSrcQList->NodeNumber;
  }
  /* Not empty destination queue */
  else
  {
    /* Find node and get its position in selected queue */
    dest_q_node_info.cllr_offset = cllr_offset;
    (void)DMA_List_FindNode(pDestQList, NULL, &dest_q_node_info);

    /* Update source queue last node CLLR to link it with destination first node */
    ((DMA_NodeTypeDef *)(dest_q_node_info.currentnode_addr))->LinkRegisters[cllr_offset] =
      ((uint32_t)pSrcQList->Head & DMA_CLLR_LA) | cllr_mask;
    pDestQList->NodeNumber += pSrcQList->NodeNumber;

    /* Check if first circular node queue is not empty */
    if (pDestQList->FirstCircularNode != NULL)
    {
      /* Find node and get its position in selected queue */
      src_q_node_info.cllr_offset = cllr_offset;
      (void)DMA_List_FindNode(pSrcQList, NULL, &src_q_node_info);

      /* Find first circular node */
      (void)DMA_List_FindNode(pDestQList, pDestQList->FirstCircularNode, &dest_q_node_info);

      /* Link last source queue node to first destination queue */
      ((DMA_NodeTypeDef *)src_q_node_info.currentnode_addr)->LinkRegisters[cllr_offset] =
        (dest_q_node_info.currentnode_addr & DMA_CLLR_LA) | cllr_mask;
    }
  }

  /* Clean the source queue variable as it is obsolete */
  DMA_List_CleanQueue(pSrcQList);

  /* Update the destination queue error code */
  pDestQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the destination queue state */
  pDestQList->State = HAL_DMA_QUEUE_STATE_READY;

  /* Prevent MISRA-C2012-Rule-2.2_b */
  UNUSED(src_q_node_info);

  return HAL_OK;
}

/**
  * @brief  Set circular mode configuration for linked-list queue.
  * @param  pQList             : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @param  pFirstCircularNode : Pointer to a DMA_NodeTypeDef structure that contains linked-list first circular node
  *                              registers configurations.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_SetCircularModeConfig(DMA_QListTypeDef *const pQList,
                                                       DMA_NodeTypeDef *const pFirstCircularNode)
{
  uint32_t cllr_mask;
  uint32_t cllr_offset;
  DMA_NodeInQInfoTypeDef node_info;

  /* Check the queue and the first circular node parameters */
  if ((pQList == NULL) || (pFirstCircularNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the queue */
  if (pQList->Head == NULL)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;

    return HAL_ERROR;
  }

  /* Check queue circular mode */
  if (pQList->FirstCircularNode != NULL)
  {
    if (pQList->FirstCircularNode == pFirstCircularNode)
    {
      return HAL_OK;
    }
    else
    {
      /* Update the queue error code */
      pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

      return HAL_ERROR;
    }
  }

  /* Check queue type */
  if (pQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Get CLLR register mask and offset */
  DMA_List_GetCLLRNodeInfo(pFirstCircularNode, &cllr_mask, &cllr_offset);

  /* Find the first circular node and get its position in selected queue */
  node_info.cllr_offset = cllr_offset;
  if (DMA_List_FindNode(pQList, pFirstCircularNode, &node_info) == 0U)
  {
    /* Find the last queue node and get its position in selected queue */
    (void)DMA_List_FindNode(pQList, NULL, &node_info);

    /* Set circular mode */
    ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset] =
      ((uint32_t)pFirstCircularNode & DMA_CLLR_LA) | cllr_mask;

    /* Update first circular node in queue */
    pQList->FirstCircularNode = pFirstCircularNode;
  }
  else
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NOTFOUND;

    return HAL_ERROR;
  }

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_READY;

  /* Prevent MISRA-C2012-Rule-2.2_b */
  UNUSED(node_info);

  return HAL_OK;
}

/**
  * @brief  Set circular mode for linked-list queue.
  * @param  pQList : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_SetCircularMode(DMA_QListTypeDef *const pQList)
{
  uint32_t cllr_mask;
  uint32_t cllr_offset;
  DMA_NodeInQInfoTypeDef node_info;

  /* Check the queue parameter */
  if (pQList == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the queue */
  if (pQList->Head == NULL)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;

    return HAL_ERROR;
  }

  /* Check queue circular mode */
  if (pQList->FirstCircularNode != NULL)
  {
    if (pQList->FirstCircularNode == pQList->Head)
    {
      return HAL_OK;
    }
    else
    {
      /* Update the queue error code */
      pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

      return HAL_ERROR;
    }
  }

  /* Check queue type */
  if (pQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Get CLLR register mask and offset */
  DMA_List_GetCLLRNodeInfo(pQList->Head, &cllr_mask, &cllr_offset);

  /* Find the last queue node and get its position in selected queue */
  node_info.cllr_offset = cllr_offset;
  (void)DMA_List_FindNode(pQList, NULL, &node_info);

  /* Set circular mode */
  ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset] =
    ((uint32_t)pQList->Head & DMA_CLLR_LA) | cllr_mask;

  /* Update linked-list circular state */
  pQList->FirstCircularNode = pQList->Head;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_READY;

  /* Prevent MISRA-C2012-Rule-2.2_b */
  UNUSED(node_info);

  return HAL_OK;
}

/**
  * @brief  Clear circular mode for linked-list queue.
  * @param  pQList : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_ClearCircularMode(DMA_QListTypeDef *const pQList)
{
  uint32_t cllr_offset;
  DMA_NodeInQInfoTypeDef node_info;

  /* Check the queue parameter */
  if (pQList == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the queue */
  if (pQList->Head == NULL)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;

    return HAL_ERROR;
  }

  /* Check queue circular mode */
  if (pQList->FirstCircularNode == NULL)
  {
    return HAL_OK;
  }

  /* Check queue type */
  if (pQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

    return HAL_ERROR;
  }

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Get CLLR register offset */
  DMA_List_GetCLLRNodeInfo(pQList->Head, NULL, &cllr_offset);

  /* Find the last queue node and get its position in selected queue */
  node_info.cllr_offset = cllr_offset;
  (void)DMA_List_FindNode(pQList, NULL, &node_info);

  /* Clear circular mode */
  ((DMA_NodeTypeDef *)(node_info.currentnode_addr))->LinkRegisters[cllr_offset] = 0U;

  /* Update linked-list circular configuration */
  pQList->FirstCircularNode = NULL;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_READY;

  /* Prevent MISRA-C2012-Rule-2.2_b */
  UNUSED(node_info);

  return HAL_OK;
}

/**
  * @brief  Convert a linked-list queue to dynamic (Optimized DMA queue execution).
  * @param  pQList : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_ConvertQToDynamic(DMA_QListTypeDef *const pQList)
{
  uint32_t cllr_offset;
  uint32_t currentnode_addr;
  DMA_NodeTypeDef context_node;
  DMA_NodeInQInfoTypeDef node_info;

  /* Check the queue parameter */
  if (pQList == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the queue */
  if (pQList->Head == NULL)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;

    return HAL_ERROR;
  }

  /* Check if queue is dynamic */
  if (pQList->Type == QUEUE_TYPE_DYNAMIC)
  {
    return HAL_OK;
  }

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Get CLLR register mask and offset */
  DMA_List_GetCLLRNodeInfo(pQList->Head, NULL, &cllr_offset);

  /* Check queue circularity */
  if (pQList->FirstCircularNode != 0U)
  {
    /* Find the last queue node and get its position in selected queue */
    node_info.cllr_offset = cllr_offset;
    (void)DMA_List_FindNode(pQList, NULL, &node_info);
  }

  /* Set current node address */
  currentnode_addr = (uint32_t)pQList->Head;

  /* Store register value */
  DMA_List_FillNode(pQList->Head, &context_node);

  /* Convert all nodes to dyncamic (Bypass head node) */
  for (uint32_t node_count = 1U; node_count < pQList->NodeNumber; node_count++)
  {
    /* Update node address */
    MODIFY_REG(currentnode_addr, DMA_CLLR_LA, (context_node.LinkRegisters[cllr_offset] & DMA_CLLR_LA));

    /* Bypass the first circular node when first circular node isn't the last queue node */
    if (((uint32_t)pQList->FirstCircularNode != 0U)                         &&
        ((uint32_t)pQList->FirstCircularNode != node_info.currentnode_addr) &&
        ((uint32_t)pQList->FirstCircularNode == currentnode_addr))
    {
      /* Copy first circular node to context node */
      DMA_List_FillNode(pQList->FirstCircularNode, &context_node);
    }
    else
    {
      /* Convert current node to dynamic */
      DMA_List_ConvertNodeToDynamic((uint32_t)&context_node, currentnode_addr, (cllr_offset + 1U));
    }
  }

  /* Check if first circular node is the last node queue */
  if (((uint32_t)pQList->FirstCircularNode != 0U) &&
      ((uint32_t)pQList->FirstCircularNode != node_info.currentnode_addr))
  {
    /* Update all queue nodes CLLR */
    DMA_List_UpdateDynamicQueueNodesCLLR(pQList, LASTNODE_ISNOT_CIRCULAR);
  }
  else
  {
    /* Update all queue nodes CLLR */
    DMA_List_UpdateDynamicQueueNodesCLLR(pQList, LASTNODE_IS_CIRCULAR);
  }

  /* Set queue type */
  pQList->Type = QUEUE_TYPE_DYNAMIC;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Convert a linked-list queue to static (Not optimized DMA queue execution).
  * @param  pQList : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_ConvertQToStatic(DMA_QListTypeDef *const pQList)
{
  uint32_t cllr_offset;
  uint32_t currentnode_addr;
  DMA_NodeTypeDef context_node;

  /* Check the queue parameter */
  if (pQList == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the queue */
  if (pQList->Head == NULL)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;

    return HAL_ERROR;
  }

  /* Check if queue is static */
  if (pQList->Type == QUEUE_TYPE_STATIC)
  {
    return HAL_OK;
  }

  /* Set current node address */
  currentnode_addr = (uint32_t)pQList->Head;

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Get CLLR register mask and offset */
  DMA_List_GetCLLRNodeInfo(pQList->Head, NULL, &cllr_offset);

  /* Set all CLLR queue nodes to their default positions */
  DMA_List_UpdateStaticQueueNodesCLLR(pQList, UPDATE_CLLR_POSITION);

  /* Convert all nodes to static (Bypass head node) */
  for (uint32_t node_count = 1U; node_count < pQList->NodeNumber; node_count++)
  {
    /* Update context node register values */
    DMA_List_FillNode((DMA_NodeTypeDef *)currentnode_addr, &context_node);

    /* Update node address */
    MODIFY_REG(currentnode_addr, DMA_CLLR_LA, (context_node.LinkRegisters[cllr_offset] & DMA_CLLR_LA));

    /* Convert current node to static */
    DMA_List_ConvertNodeToStatic((uint32_t)&context_node, currentnode_addr, (cllr_offset + 1U));
  }

  /* Set all CLLR queue nodes to their default values */
  DMA_List_UpdateStaticQueueNodesCLLR(pQList, UPDATE_CLLR_VALUE);

  /* Set queue type */
  pQList->Type = QUEUE_TYPE_STATIC;

  /* Update the queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Update the queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Link linked-list queue to a DMA channel.
  * @param  hdma   : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                  specified DMA Channel.
  * @param  pQList : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_LinkQ(DMA_HandleTypeDef *const hdma,
                                       DMA_QListTypeDef *const pQList)
{
  HAL_DMA_StateTypeDef state;

  /* Check the DMA channel handle and the queue parameters */
  if ((hdma == NULL) || (pQList == NULL))
  {
    return HAL_ERROR;
  }

  /* Get DMA state */
  state = hdma->State;

  /* Check DMA channel state */
  if ((hdma->State == HAL_DMA_STATE_BUSY) || (state == HAL_DMA_STATE_SUSPEND))
  {
    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_BUSY;

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }

  /* Check queue state */
  if (pQList->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_BUSY;

    return HAL_ERROR;
  }

  /* Check linearity compatibility */
  if ((IS_DMA_2D_ADDRESSING_INSTANCE(hdma->Instance) == 0U) &&
      ((pQList->Head->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR))
  {
    /* Update the queue error code */
    pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_UNSUPPORTED;

    return HAL_ERROR;
  }

  /* Check circularity compatibility */
  if (hdma->Mode == DMA_LINKEDLIST_CIRCULAR)
  {
    /* Check first circular node */
    if (pQList->FirstCircularNode == NULL)
    {
      /* Update the queue error code */
      pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

      return HAL_ERROR;
    }
  }
  else
  {
    /* Check first circular node */
    if (pQList->FirstCircularNode != NULL)
    {
      /* Update the queue error code */
      pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;

      return HAL_ERROR;
    }
  }

  /* Register queue to DMA handle */
  hdma->LinkedListQueue = pQList;

  return HAL_OK;
}

/**
  * @brief  Unlink linked-list queue from a DMA channel.
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_List_UnLinkQ(DMA_HandleTypeDef *const hdma)
{
  HAL_DMA_StateTypeDef state;

  /* Check the DMA channel parameter */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Get DMA state */
  state = hdma->State;

  /* Check DMA channel state */
  if ((hdma->State == HAL_DMA_STATE_BUSY) || (state == HAL_DMA_STATE_SUSPEND))
  {
    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_BUSY;

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }

  /* Clear queue information from DMA channel handle */
  hdma->LinkedListQueue = NULL;

  return HAL_OK;
}
/**
  * @}
  */

/** @addtogroup DMAEx_Exported_Functions_Group4
  *
@verbatim
  ======================================================================================================================
             ##### Data handling, repeated block and trigger configuration functions #####
  ======================================================================================================================
    [..]
      This section provides functions allowing to :
      (+) Configure DMA channel data handling.
      (+) Configure DMA channel repeated block.
      (+) Configure DMA channel trigger.

    [..]
      (+) The HAL_DMAEx_ConfigDataHandling() function allows to configure DMA channel data handling.
              (++) GPDMA or HPDMA data handling : byte-based reordering, packing/unpacking, padding/truncation,
                                                  sign extension and left/right alignment.

      (+) The HAL_DMAEx_ConfigTrigger() function allows to configure DMA channel HW triggers.

      (+) The HAL_DMAEx_ConfigRepeatBlock() function allows to configure DMA channel repeated block.
              (++) This feature is available only for channel that supports 2 dimensions addressing capability.

@endverbatim
  * @{
  */

/**
  * @brief  Configure the DMA channel data handling according to the specified parameters in the
  *         DMA_DataHandlingConfTypeDef.
  * @param  hdma                : Pointer to a DMA_HandleTypeDef structure that contains the configuration information
  *                               for the specified DMA Channel.
  * @param  pConfigDataHandling : Pointer to a DMA_DataHandlingConfTypeDef structure that contains the data handling
  *                               configuration.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_ConfigDataHandling(DMA_HandleTypeDef *const hdma,
                                               DMA_DataHandlingConfTypeDef const *const pConfigDataHandling)
{
  /* Check the DMA peripheral handle and data handling parameters */
  if ((hdma == NULL) || (pConfigDataHandling == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_DATA_ALIGNMENT(pConfigDataHandling->DataAlignment));
  assert_param(IS_DMA_DATA_EXCHANGE(pConfigDataHandling->DataExchange));

  /* Check DMA channel state */
  if (hdma->State == HAL_DMA_STATE_READY)
  {
    MODIFY_REG(hdma->Instance->CTR1, (DMA_CTR1_DWX | DMA_CTR1_DHX | DMA_CTR1_DBX | DMA_CTR1_SBX | DMA_CTR1_PAM),
               (pConfigDataHandling->DataAlignment | pConfigDataHandling->DataExchange));
  }
  else
  {
    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_BUSY;

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Configure the DMA channel trigger according to the specified parameters in the DMA_TriggerConfTypeDef.
  * @param  hdma           : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for
  *                          the specified DMA Channel.
  * @param  pConfigTrigger : Pointer to a DMA_TriggerConfTypeDef structure that contains the trigger configuration.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_ConfigTrigger(DMA_HandleTypeDef *const hdma,
                                          DMA_TriggerConfTypeDef const *const pConfigTrigger)
{
  /* Check the DMA peripheral handle and trigger parameters */
  if ((hdma == NULL) || (pConfigTrigger == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_ALL_INSTANCE(hdma->Instance));
  assert_param(IS_DMA_TRIGGER_POLARITY(pConfigTrigger->TriggerPolarity));
  assert_param(IS_DMA_TRIGGER_MODE(pConfigTrigger->TriggerMode));
  assert_param(IS_DMA_TRIGGER_SELECTION(pConfigTrigger->TriggerSelection));

  /* Check DMA channel state */
  if (hdma->State == HAL_DMA_STATE_READY)
  {
    MODIFY_REG(hdma->Instance->CTR2, (DMA_CTR2_TRIGPOL | DMA_CTR2_TRIGSEL | DMA_CTR2_TRIGM),
               (pConfigTrigger->TriggerPolarity | pConfigTrigger->TriggerMode |
                (pConfigTrigger->TriggerSelection << DMA_CTR2_TRIGSEL_Pos)));
  }
  else
  {
    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_BUSY;

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Configure the DMA channel repeated block according to the specified parameters in the
  *         DMA_RepeatBlockConfTypeDef.
  * @param  hdma               : Pointer to a DMA_HandleTypeDef structure that contains the configuration information
  *                              for the specified DMA Channel.
  * @param  pConfigRepeatBlock : Pointer to a DMA_RepeatBlockConfTypeDef structure that contains the repeated block
  *                              configuration.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_ConfigRepeatBlock(DMA_HandleTypeDef *const hdma,
                                              DMA_RepeatBlockConfTypeDef const *const pConfigRepeatBlock)
{
  uint32_t tmpreg1;
  uint32_t tmpreg2;

  /* Check the DMA peripheral handle and repeated block parameters */
  if ((hdma == NULL) || (pConfigRepeatBlock == NULL))
  {
    return HAL_ERROR;
  }

  /* Check parameters */
  assert_param(IS_DMA_2D_ADDRESSING_INSTANCE(hdma->Instance));
  assert_param(IS_DMA_REPEAT_COUNT(pConfigRepeatBlock->RepeatCount));
  assert_param(IS_DMA_BURST_ADDR_OFFSET(pConfigRepeatBlock->SrcAddrOffset));
  assert_param(IS_DMA_BURST_ADDR_OFFSET(pConfigRepeatBlock->DestAddrOffset));
  assert_param(IS_DMA_BLOCK_ADDR_OFFSET(pConfigRepeatBlock->BlkSrcAddrOffset));
  assert_param(IS_DMA_BLOCK_ADDR_OFFSET(pConfigRepeatBlock->BlkDestAddrOffset));

  /* Check DMA channel state */
  if (hdma->State == HAL_DMA_STATE_READY)
  {
    /* Store repeat block count */
    tmpreg1 = ((pConfigRepeatBlock->RepeatCount - 1U) << DMA_CBR1_BRC_Pos);

    /* Check the sign of single/burst destination address offset value */
    if (pConfigRepeatBlock->DestAddrOffset < 0)
    {
      /* Store single/burst destination address offset configuration (signed case) */
      tmpreg1 |= DMA_CBR1_DDEC;
      tmpreg2 = (uint32_t)(- pConfigRepeatBlock->DestAddrOffset);
      tmpreg2 = tmpreg2 << DMA_CTR3_DAO_Pos;
    }
    else
    {
      /* Store single/burst destination address offset configuration (unsigned case) */
      tmpreg2 = ((uint32_t)pConfigRepeatBlock->DestAddrOffset << DMA_CTR3_DAO_Pos);
    }

    /* Check the sign of single/burst source address offset value */
    if (pConfigRepeatBlock->SrcAddrOffset < 0)
    {
      /* Store single/burst source address offset configuration (signed case) */
      tmpreg1 |= DMA_CBR1_SDEC;
      tmpreg2 |= (uint32_t)(- pConfigRepeatBlock->SrcAddrOffset);
    }
    else
    {
      /* Store single/burst source address offset configuration (unsigned case) */
      tmpreg2 |= (uint32_t)pConfigRepeatBlock->SrcAddrOffset;
    }

    /* Write DMA Channel Transfer Register 3 (CTR3) */
    WRITE_REG(hdma->Instance->CTR3, tmpreg2);

    /* Check the sign of block destination address offset value */
    if (pConfigRepeatBlock->BlkDestAddrOffset < 0)
    {
      /* Store block destination address offset configuration (signed case) */
      tmpreg1 |= DMA_CBR1_BRDDEC;
      tmpreg2 = (uint32_t)(- pConfigRepeatBlock->BlkDestAddrOffset);
      tmpreg2 = tmpreg2 << DMA_CBR2_BRDAO_Pos;
    }
    else
    {
      /* Store block destination address offset configuration (unsigned case) */
      tmpreg2 = ((uint32_t)pConfigRepeatBlock->BlkDestAddrOffset << DMA_CBR2_BRDAO_Pos);
    }

    /* Check the sign of block source address offset value */
    if (pConfigRepeatBlock->BlkSrcAddrOffset < 0)
    {
      /* Store block source address offset configuration (signed case) */
      tmpreg1 |= DMA_CBR1_BRSDEC;
      tmpreg2 |= (uint32_t)(- pConfigRepeatBlock->BlkSrcAddrOffset);
    }
    else
    {
      /* Store block source address offset configuration (unsigned case) */
      tmpreg2 |= (uint32_t)pConfigRepeatBlock->BlkSrcAddrOffset;
    }

    /* Write DMA Channel block register 2 (CBR2) */
    WRITE_REG(hdma->Instance->CBR2, tmpreg2);

    /* Write DMA Channel block register 1 (CBR1) */
    WRITE_REG(hdma->Instance->CBR1, tmpreg1);
  }
  else
  {
    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_BUSY;

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }

  return HAL_OK;
}
/**
  * @}
  */

/** @addtogroup DMAEx_Exported_Functions_Group5
  *
@verbatim
  ======================================================================================================================
                         ##### Suspend and resume operation functions #####
  ======================================================================================================================
    [..]
      This section provides functions allowing to :
      (+) Suspend any ongoing DMA channel transfer.
      (+) Resume any suspended DMA channel transfer.

    [..]
      (+) The HAL_DMAEx_Suspend() function allows to suspend any ongoing DMA channel transfer in polling mode (Blocking
          mode).

      (+) The HAL_DMAEx_Suspend_IT() function allows to suspend any ongoing DMA channel transfer in interrupt mode
         (Non-blocking mode).

      (+) The HAL_DMAEx_Resume() function allows to resume any suspended DMA channel transfer.

@endverbatim
  * @{
  */

/**
  * @brief  Suspend any ongoing DMA channel transfer in polling mode (Blocking mode).
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA channel.
  * @note   After suspending a DMA channel, a check for wait until the DMA channel is effectively suspended is added. If
  *         a channel is suspended while a data transfer is ongoing, the current data will be transferred and the
  *         channel will be effectively suspended only after the transfer of this single/burst data is finished.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_Suspend(DMA_HandleTypeDef *const hdma)
{
  /* Get tick number */
  uint32_t tickstart = HAL_GetTick();

  /* Check the DMA peripheral handle */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check DMA channel state */
  if (hdma->State != HAL_DMA_STATE_BUSY)
  {
    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }
  else
  {
    /* Suspend the channel */
    hdma->Instance->CCR |= DMA_CCR_SUSP;

    /* Check if the DMA channel is suspended */
    while ((hdma->Instance->CSR & DMA_CSR_SUSPF) == 0U)
    {
      /* Check for the timeout */
      if ((HAL_GetTick() - tickstart) > HAL_TIMEOUT_DMA_ABORT)
      {
        /* Update the DMA channel error code */
        hdma->ErrorCode |= HAL_DMA_ERROR_TIMEOUT;

        /* Update the DMA channel state */
        hdma->State = HAL_DMA_STATE_ERROR;

        /* Process Unlocked */
        __HAL_UNLOCK(hdma);

        return HAL_ERROR;
      }
    }

    /* Update the DMA channel state */
    hdma->State = HAL_DMA_STATE_SUSPEND;
  }

  return HAL_OK;
}

/**
  * @brief  Suspend any ongoing DMA channel transfer in polling mode (Non-blocking mode).
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_Suspend_IT(DMA_HandleTypeDef *const hdma)
{
  /* Check the DMA peripheral handle parameter */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check DMA channel state */
  if (hdma->State != HAL_DMA_STATE_BUSY)
  {
    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }
  else
  {
    /* Suspend the DMA channel and activate suspend interrupt */
    hdma->Instance->CCR |= (DMA_CCR_SUSP | DMA_CCR_SUSPIE);
  }

  return HAL_OK;
}

/**
  * @brief  Resume any suspended DMA channel transfer.
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMAEx_Resume(DMA_HandleTypeDef *const hdma)
{
  /* Check the DMA peripheral handle parameter */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check DMA channel state */
  if (hdma->State != HAL_DMA_STATE_SUSPEND)
  {
    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }
  else
  {
    /* Resume the DMA channel */
    hdma->Instance->CCR &= (~DMA_CCR_SUSP);

    /* Clear the suspend flag */
    hdma->Instance->CFCR |= DMA_CFCR_SUSPF;

    /* Update the DMA channel state */
    hdma->State = HAL_DMA_STATE_BUSY;
  }

  return HAL_OK;
}
/**
  * @}
  */

/** @addtogroup DMAEx_Exported_Functions_Group6
  *
@verbatim
  ======================================================================================================================
                               ##### Fifo status function #####
  ======================================================================================================================
    [..]
      This section provides function allowing to get DMA channel FIFO level.

    [..]
      (+) The HAL_DMAEx_GetFifoLevel() function allows to return the number of available write beats in the FIFO, in
          units of the programmed destination data.
              (++) This API is available only for DMA channels that supports FIFO.

@endverbatim
  * @{
  */

/**
  * @brief  Get and returns the DMA channel FIFO level.
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval Returns the number of available beats in FIFO.
  */
uint32_t HAL_DMAEx_GetFifoLevel(DMA_HandleTypeDef const *const hdma)
{
  return ((hdma->Instance->CSR & DMA_CSR_FIFOL) >> DMA_CSR_FIFOL_Pos);
}
/**
  * @}
  */

/**
  * @}
  */

/* Private functions -------------------------------------------------------------------------------------------------*/
/** @defgroup DMAEx_Private_Functions DMAEx Private Functions
  * @brief    DMAEx Private Functions
  * @{
  */

/**
  * @brief  Initialize the DMA handle according to the specified parameters in the DMA_InitTypeDef.
  * @param  hdma : pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval None.
  */
static void DMA_List_Init(DMA_HandleTypeDef const *const hdma)
{
  uint32_t tmpreg;

  /* Prepare DMA Channel Control Register (CCR) value */
  tmpreg = hdma->InitLinkedList.Priority | hdma->InitLinkedList.LinkStepMode;

  /* Check DMA channel instance */
  if ((IS_HPDMA_INSTANCE(hdma->Instance) != 0U) || (IS_GPDMA_INSTANCE(hdma->Instance) != 0U))
  {
    tmpreg |= hdma->InitLinkedList.LinkAllocatedPort;
  }

  /* Write DMA Channel Control Register (CCR) */
  MODIFY_REG(hdma->Instance->CCR, DMA_CCR_PRIO | DMA_CCR_LAP | DMA_CCR_LSM, tmpreg);

  /* Write DMA Channel Control Register (CTR1) */
  WRITE_REG(hdma->Instance->CTR1, 0U);

  /* Write DMA Channel Control Register (CTR2) */
  WRITE_REG(hdma->Instance->CTR2, hdma->InitLinkedList.TransferEventMode);

  /* Write DMA Channel Control Register (CBR1) */
  WRITE_REG(hdma->Instance->CBR1, 0U);

  /* Write DMA Channel Control Register (CSAR) */
  WRITE_REG(hdma->Instance->CSAR, 0U);

  /* Write DMA Channel Control Register (CDAR) */
  WRITE_REG(hdma->Instance->CDAR, 0U);

  /* If 2D Addressing is supported by current channel */
  if (IS_DMA_2D_ADDRESSING_INSTANCE(hdma->Instance) != 0U)
  {
    /* Write DMA Channel Control Register (CTR3) */
    WRITE_REG(hdma->Instance->CTR3, 0U);

    /* Write DMA Channel Control Register (CBR2) */
    WRITE_REG(hdma->Instance->CBR2, 0U);
  }

  /* Write DMA Channel linked-list address register (CLLR) */
  WRITE_REG(hdma->Instance->CLLR, 0U);
}

/**
  * @brief  Build a DMA channel node according to the specified parameters in the DMA_NodeConfTypeDef.
  * @param  pNodeConfig : Pointer to a DMA_NodeConfTypeDef structure that contains the configuration information for the
  *                       specified DMA linked-list Node.
  * @param  pNode       : Pointer to a DMA_NodeTypeDef structure that contains linked-list node registers
  *                       configurations.
  * @retval None.
  */
static void DMA_List_BuildNode(DMA_NodeConfTypeDef const *const pNodeConfig,
                               DMA_NodeTypeDef *const pNode)
{
  int32_t blockoffset;

  /* Update CTR1 register value ***************************************************************************************/
  /* Prepare DMA channel transfer register (CTR1) value */
  pNode->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET] = pNodeConfig->Init.DestInc                     |
                                                   pNodeConfig->Init.DestDataWidth               |
                                                   pNodeConfig->DataHandlingConfig.DataAlignment |
                                                   pNodeConfig->Init.SrcInc                      |
                                                   pNodeConfig->Init.SrcDataWidth;

#if defined (CPU_IN_SECURE_STATE)
  /* set source channel security attribute */
  if (pNodeConfig->SrcSecure == DMA_CHANNEL_SRC_SEC)
  {
    pNode->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET] |= DMA_CTR1_SSEC;
  }

  /* set destination channel security attribute */
  if (pNodeConfig->DestSecure == DMA_CHANNEL_DEST_SEC)
  {
    pNode->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET] |= DMA_CTR1_DSEC;
  }
#endif /* CPU_IN_SECURE_STATE */

  /* Add parameters related to DMA configuration */
  if ((pNodeConfig->NodeType & (DMA_CHANNEL_TYPE_GPDMA | DMA_CHANNEL_TYPE_HPDMA)) != 0U)
  {
    /* Prepare DMA channel transfer register (CTR1) value */
    pNode->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET] |=
      (pNodeConfig->Init.TransferAllocatedPort | pNodeConfig->DataHandlingConfig.DataExchange |
       (((pNodeConfig->Init.DestBurstLength - 1U) << DMA_CTR1_DBL_1_Pos) & DMA_CTR1_DBL_1)    |
       (((pNodeConfig->Init.SrcBurstLength - 1U) << DMA_CTR1_SBL_1_Pos) & DMA_CTR1_SBL_1));
  }
  /*********************************************************************************** CTR1 register value is updated */


  /* Update CTR2 register value ***************************************************************************************/
  /* Prepare DMA channel transfer register 2 (CTR2) value */
  pNode->LinkRegisters[NODE_CTR2_DEFAULT_OFFSET] = pNodeConfig->Init.TransferEventMode |
                                                   (pNodeConfig->Init.Request & (DMA_CTR2_REQSEL | DMA_CTR2_SWREQ));

  /* Check for memory to peripheral transfer */
  if ((pNodeConfig->Init.Direction) == DMA_MEMORY_TO_PERIPH)
  {
    /* Check for GPDMA OR HPDMA */
    if ((pNodeConfig->NodeType & (DMA_CHANNEL_TYPE_GPDMA | DMA_CHANNEL_TYPE_HPDMA)) != 0U)
    {
      pNode->LinkRegisters[NODE_CTR2_DEFAULT_OFFSET] |= DMA_CTR2_DREQ;
    }
  }
  /* Memory to memory transfer */
  else if ((pNodeConfig->Init.Direction) == DMA_MEMORY_TO_MEMORY)
  {
    pNode->LinkRegisters[NODE_CTR2_DEFAULT_OFFSET] |= DMA_CTR2_SWREQ;
  }
  else
  {
    /* Prevent MISRA-C2012-Rule-15.7 */
  }

  /* Configure HW Peripheral flow control selection */
  pNode->LinkRegisters[NODE_CTR2_DEFAULT_OFFSET] |= pNodeConfig->Init.Mode;

  /* Check if trigger feature is active */
  if (pNodeConfig->TriggerConfig.TriggerPolarity != DMA_TRIG_POLARITY_MASKED)
  {
    /* Prepare DMA channel transfer register 2 (CTR2) value */
    pNode->LinkRegisters[NODE_CTR2_DEFAULT_OFFSET] |=
      pNodeConfig->TriggerConfig.TriggerMode | pNodeConfig->TriggerConfig.TriggerPolarity |
      ((pNodeConfig->TriggerConfig.TriggerSelection << DMA_CTR2_TRIGSEL_Pos) & DMA_CTR2_TRIGSEL);
  }
  /*********************************************************************************** CTR2 register value is updated */


  /* Update CBR1 register value ***************************************************************************************/
  /* Prepare DMA channel block register 1 (CBR1) value */
  pNode->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] = (pNodeConfig->DataSize & DMA_CBR1_BNDT);

  /* If 2D addressing is supported by the selected DMA channel */
  if ((pNodeConfig->NodeType & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
  {
    /* Set the new CBR1 Register value */
    pNode->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] |=
      (((pNodeConfig->RepeatBlockConfig.RepeatCount - 1U) << DMA_CBR1_BRC_Pos) & DMA_CBR1_BRC);

    /* If the source address offset is negative, set SDEC bit */
    if (pNodeConfig->RepeatBlockConfig.SrcAddrOffset < 0)
    {
      pNode->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] |= DMA_CBR1_SDEC;
    }
    else
    {
      pNode->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] &= (~DMA_CBR1_SDEC);
    }

    /* If the destination address offset is negative, set DDEC bit */
    if (pNodeConfig->RepeatBlockConfig.DestAddrOffset < 0)
    {
      pNode->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] |= DMA_CBR1_DDEC;
    }
    else
    {
      pNode->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] &= (~DMA_CBR1_DDEC);
    }

    /* If the repeated block source address offset is negative, set BRSEC bit */
    if (pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset < 0)
    {
      pNode->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] |= DMA_CBR1_BRSDEC;
    }
    else
    {
      pNode->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] &= (~DMA_CBR1_BRSDEC);
    }

    /* if the repeated block destination address offset is negative, set BRDEC bit */
    if (pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset < 0)
    {
      pNode->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] |= DMA_CBR1_BRDDEC;
    }
    else
    {
      pNode->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] &= (~DMA_CBR1_BRDDEC);
    }
  }
  /*********************************************************************************** CBR1 register value is updated */


  /* Update CSAR register value ***************************************************************************************/
  pNode->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET] = pNodeConfig->SrcAddress;
  /*********************************************************************************** CSAR register value is updated */


  /* Update CDAR register value ***************************************************************************************/
  pNode->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = pNodeConfig->DstAddress;
  /*********************************************************************************** CDAR register value is updated */

  /* Check if the selected channel is 2D addressing */
  if ((pNodeConfig->NodeType & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
  {
    /* Update CTR3 register value *************************************************************************************/
    /* Write new CTR3 Register value : source address offset */
    if (pNodeConfig->RepeatBlockConfig.SrcAddrOffset < 0)
    {
      blockoffset = (- pNodeConfig->RepeatBlockConfig.SrcAddrOffset);
      pNode->LinkRegisters[NODE_CTR3_DEFAULT_OFFSET] = ((uint32_t)blockoffset & DMA_CTR3_SAO);
    }
    else
    {
      pNode->LinkRegisters[NODE_CTR3_DEFAULT_OFFSET] =
        ((uint32_t)pNodeConfig->RepeatBlockConfig.SrcAddrOffset & DMA_CTR3_SAO);
    }

    /* Write new CTR3 Register value : destination address offset */
    if (pNodeConfig->RepeatBlockConfig.DestAddrOffset < 0)
    {
      blockoffset = (- pNodeConfig->RepeatBlockConfig.DestAddrOffset);
      pNode->LinkRegisters[NODE_CTR3_DEFAULT_OFFSET] |= (((uint32_t)blockoffset << DMA_CTR3_DAO_Pos) & DMA_CTR3_DAO);
    }
    else
    {
      pNode->LinkRegisters[NODE_CTR3_DEFAULT_OFFSET] |=
        (((uint32_t)pNodeConfig->RepeatBlockConfig.DestAddrOffset << DMA_CTR3_DAO_Pos) & DMA_CTR3_DAO);
    }
    /********************************************************************************* CTR3 register value is updated */


    /* Update CBR2 register value *************************************************************************************/
    /* Write new CBR2 Register value : repeated block source address offset */
    if (pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset < 0)
    {
      blockoffset = (- pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset);
      pNode->LinkRegisters[NODE_CBR2_DEFAULT_OFFSET] = ((uint32_t)blockoffset & DMA_CBR2_BRSAO);
    }
    else
    {
      pNode->LinkRegisters[NODE_CBR2_DEFAULT_OFFSET] =
        ((uint32_t)pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset & DMA_CBR2_BRSAO);
    }

    /* Write new CBR2 Register value : repeated block destination address offset */
    if (pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset < 0)
    {
      blockoffset = (- pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset);
      pNode->LinkRegisters[NODE_CBR2_DEFAULT_OFFSET] |=
        (((uint32_t)blockoffset & DMA_CBR2_BRSAO) << DMA_CBR2_BRDAO_Pos);
    }
    else
    {
      pNode->LinkRegisters[NODE_CBR2_DEFAULT_OFFSET] |=
        (((uint32_t)pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset << DMA_CBR2_BRDAO_Pos) & DMA_CBR2_BRDAO);
    }
    /********************************************************************************* CBR2 register value is updated */

    /* Update CLLR register value *************************************************************************************/
    /* Reset CLLR Register value : channel linked-list address register offset */
    pNode->LinkRegisters[NODE_CLLR_2D_DEFAULT_OFFSET] = 0U;
    /********************************************************************************* CLLR register value is cleared */
  }
  else
  {
    /* Update CLLR register value *************************************************************************************/
    /* Reset CLLR Register value : channel linked-list address register offset */
    pNode->LinkRegisters[NODE_CLLR_LINEAR_DEFAULT_OFFSET] = 0U;
    /********************************************************************************* CLLR register value is cleared */
  }

  /* Update node information value ************************************************************************************/
  /* Set node information */
  pNode->NodeInfo = pNodeConfig->NodeType;
  if ((pNodeConfig->NodeType & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
  {
    pNode->NodeInfo |= (NODE_CLLR_2D_DEFAULT_OFFSET << NODE_CLLR_IDX_POS);
  }
  else
  {
    pNode->NodeInfo |= (NODE_CLLR_LINEAR_DEFAULT_OFFSET << NODE_CLLR_IDX_POS);
  }
  /******************************************************************************** Node information value is updated */
}

/**
  * @brief  Get a DMA channel node configuration.
  * @param  pNodeConfig : Pointer to a DMA_NodeConfTypeDef structure that contains the configuration information for the
  *                       specified DMA linked-list Node.
  * @param  pNode       : Pointer to a DMA_NodeTypeDef structure that contains linked-list node registers
  *                       configurations.
  * @retval None.
  */
static void DMA_List_GetNodeConfig(DMA_NodeConfTypeDef *const pNodeConfig,
                                   DMA_NodeTypeDef const *const pNode)
{
  uint16_t offset;

  /* Get node information *********************************************************************************************/
  pNodeConfig->NodeType = (pNode->NodeInfo & NODE_TYPE_MASK);
  /*************************************************************************************** Node type value is updated */


  /* Get CTR1 fields values *******************************************************************************************/
  pNodeConfig->Init.SrcInc                      = pNode->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET]   & DMA_CTR1_SINC;
  pNodeConfig->Init.DestInc                     = pNode->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET]   & DMA_CTR1_DINC;
  pNodeConfig->Init.SrcDataWidth                = pNode->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET]   & DMA_CTR1_SDW_LOG2;
  pNodeConfig->Init.DestDataWidth               = pNode->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET]   & DMA_CTR1_DDW_LOG2;
  pNodeConfig->Init.SrcBurstLength              = ((pNode->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET] &
                                                    DMA_CTR1_SBL_1) >> DMA_CTR1_SBL_1_Pos) + 1U;
  pNodeConfig->Init.DestBurstLength             = ((pNode->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET] &
                                                    DMA_CTR1_DBL_1) >> DMA_CTR1_DBL_1_Pos) + 1U;
  pNodeConfig->Init.TransferAllocatedPort       = pNode->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET]   &
                                                  (DMA_CTR1_SAP | DMA_CTR1_DAP);
  pNodeConfig->DataHandlingConfig.DataExchange  = pNode->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET]   &
                                                  (DMA_CTR1_SBX | DMA_CTR1_DBX | DMA_CTR1_DHX | DMA_CTR1_DWX);
  pNodeConfig->DataHandlingConfig.DataAlignment = pNode->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET]   & DMA_CTR1_PAM;
#if defined (CPU_IN_SECURE_STATE)
  if ((pNode->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET] & DMA_CTR1_SSEC) != 0U)
  {
    pNodeConfig->SrcSecure = DMA_CHANNEL_SRC_SEC;
  }
  else
  {
    pNodeConfig->SrcSecure = DMA_CHANNEL_SRC_NSEC;
  }

  if ((pNode->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET] & DMA_CTR1_DSEC) != 0U)
  {
    pNodeConfig->DestSecure = DMA_CHANNEL_DEST_SEC;
  }
  else
  {
    pNodeConfig->DestSecure = DMA_CHANNEL_DEST_NSEC;
  }
#endif /* CPU_IN_SECURE_STATE */
  /*********************************************************************************** CTR1 fields values are updated */


  /* Get CTR2 fields values *******************************************************************************************/
  if ((pNode->LinkRegisters[NODE_CTR2_DEFAULT_OFFSET] & DMA_CTR2_SWREQ) != 0U)
  {
    pNodeConfig->Init.Request   = DMA_REQUEST_SW;
    pNodeConfig->Init.Direction = DMA_MEMORY_TO_MEMORY;
  }
  else
  {
    pNodeConfig->Init.Request   = pNode->LinkRegisters[NODE_CTR2_DEFAULT_OFFSET] & DMA_CTR2_REQSEL;

    if ((pNode->LinkRegisters[NODE_CTR2_DEFAULT_OFFSET] & DMA_CTR2_DREQ) != 0U)
    {
      pNodeConfig->Init.Direction = DMA_MEMORY_TO_PERIPH;
    }
    else
    {
      pNodeConfig->Init.Direction = DMA_PERIPH_TO_MEMORY;
    }
  }

  pNodeConfig->Init.BlkHWRequest              = (pNode->LinkRegisters[NODE_CTR2_DEFAULT_OFFSET] & DMA_CTR2_BREQ);
  pNodeConfig->TriggerConfig.TriggerMode      = pNode->LinkRegisters[NODE_CTR2_DEFAULT_OFFSET]  & DMA_CTR2_TRIGM;
  pNodeConfig->TriggerConfig.TriggerPolarity  = pNode->LinkRegisters[NODE_CTR2_DEFAULT_OFFSET]  & DMA_CTR2_TRIGPOL;
  pNodeConfig->TriggerConfig.TriggerSelection = (pNode->LinkRegisters[NODE_CTR2_DEFAULT_OFFSET] &
                                                 DMA_CTR2_TRIGSEL) >> DMA_CTR2_TRIGSEL_Pos;
  pNodeConfig->Init.TransferEventMode         = pNode->LinkRegisters[NODE_CTR2_DEFAULT_OFFSET]  & DMA_CTR2_TCEM;
  /*********************************************************************************** CTR2 fields values are updated */


  /* Get CBR1 fields **************************************************************************************************/
  pNodeConfig->DataSize = pNode->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] & DMA_CBR1_BNDT;

  if ((pNodeConfig->NodeType & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
  {
    pNodeConfig->RepeatBlockConfig.RepeatCount =
      ((pNode->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] & DMA_CBR1_BRC) >> DMA_CBR1_BRC_Pos) + 1U;
  }
  else
  {
    pNodeConfig->RepeatBlockConfig.RepeatCount = 1U;
  }
  /*********************************************************************************** CBR1 fields values are updated */


  /* Get CSAR field ***************************************************************************************************/
  pNodeConfig->SrcAddress = pNode->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET];
  /************************************************************************************** CSAR field value is updated */


  /* Get CDAR field ***************************************************************************************************/
  pNodeConfig->DstAddress = pNode->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET];
  /************************************************************************************** CDAR field value is updated */

  /* Check if the selected channel is 2D addressing */
  if ((pNodeConfig->NodeType & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
  {
    /* Get CTR3 field *************************************************************************************************/
    offset = (uint16_t)(pNode->LinkRegisters[NODE_CTR3_DEFAULT_OFFSET] & DMA_CTR3_SAO);
    pNodeConfig->RepeatBlockConfig.SrcAddrOffset  = (int32_t)offset;

    offset = (uint16_t)((pNode->LinkRegisters[NODE_CTR3_DEFAULT_OFFSET] & DMA_CTR3_DAO) >> DMA_CTR3_DAO_Pos);
    pNodeConfig->RepeatBlockConfig.DestAddrOffset = (int32_t)offset;

    if ((pNode->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] & DMA_CBR1_SDEC) != 0U)
    {
      pNodeConfig->RepeatBlockConfig.SrcAddrOffset *= (-1);
    }

    if ((pNode->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] & DMA_CBR1_DDEC) != 0U)
    {
      pNodeConfig->RepeatBlockConfig.DestAddrOffset *= (-1);
    }
    /************************************************************************************ CTR3 field value is updated */


    /* Get CBR2 fields ************************************************************************************************/
    offset = (uint16_t)(pNode->LinkRegisters[NODE_CBR2_DEFAULT_OFFSET] & DMA_CBR2_BRSAO);
    pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset = (int32_t)offset;

    offset = (uint16_t)((pNode->LinkRegisters[NODE_CBR2_DEFAULT_OFFSET] & DMA_CBR2_BRDAO) >> DMA_CBR2_BRDAO_Pos);
    pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset = (int32_t)offset;

    if ((pNode->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] & DMA_CBR1_BRSDEC) != 0U)
    {
      pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset *= (-1);
    }

    if ((pNode->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] & DMA_CBR1_BRDDEC) != 0U)
    {
      pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset *= (-1);
    }
    /************************************************************************************ CBR2 field value is updated */
  }
  else
  {
    /* Get CTR3 field *************************************************************************************************/
    pNodeConfig->RepeatBlockConfig.SrcAddrOffset     = 0;
    pNodeConfig->RepeatBlockConfig.DestAddrOffset    = 0;
    /************************************************************************************ CTR3 field value is updated */


    /* Get CBR2 fields ************************************************************************************************/
    pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset  = 0;
    pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset = 0;
    /************************************************************************************ CBR2 field value is updated */
  }
}

/**
  * @brief  Check nodes base addresses compatibility.
  * @param  pNode1 : Pointer to a DMA_NodeTypeDef structure that contains linked-list node 1 registers configurations.
  * @param  pNode2 : Pointer to a DMA_NodeTypeDef structure that contains linked-list node 2 registers configurations.
  * @param  pNode3 : Pointer to a DMA_NodeTypeDef structure that contains linked-list node 3 registers configurations.
  * @retval Return 0 when nodes addresses are compatible, 1 otherwise.
  */
static uint32_t DMA_List_CheckNodesBaseAddresses(DMA_NodeTypeDef const *const pNode1,
                                                 DMA_NodeTypeDef const *const pNode2,
                                                 DMA_NodeTypeDef const *const pNode3)
{
  uint32_t temp = (((uint32_t)pNode1 | (uint32_t)pNode2 | (uint32_t)pNode3) & DMA_CLBAR_LBA);
  uint32_t ref  = 0U;

  /* Check node 1 address */
  if ((uint32_t)pNode1 != 0U)
  {
    ref = (uint32_t)pNode1;
  }
  /* Check node 2 address */
  else if ((uint32_t)pNode2 != 0U)
  {
    ref = (uint32_t)pNode2;
  }
  /* Check node 3 address */
  else if ((uint32_t)pNode3 != 0U)
  {
    ref = (uint32_t)pNode3;
  }
  else
  {
    /* Prevent MISRA-C2012-Rule-15.7 */
  }

  /* Check addresses compatibility */
  if (temp != ((uint32_t)ref & DMA_CLBAR_LBA))
  {
    return 1U;
  }

  return 0U;
}

/**
  * @brief  Check nodes types compatibility.
  * @param  pNode1 : Pointer to a DMA_NodeTypeDef structure that contains linked-list node 1 registers configurations.
  * @param  pNode2 : Pointer to a DMA_NodeTypeDef structure that contains linked-list node 2 registers configurations.
  * @param  pNode3 : Pointer to a DMA_NodeTypeDef structure that contains linked-list node 3 registers configurations.
  * @retval Return 0 when nodes types are compatible, otherwise nodes types are not compatible.
  */
static uint32_t DMA_List_CheckNodesTypes(DMA_NodeTypeDef const *const pNode1,
                                         DMA_NodeTypeDef const *const pNode2,
                                         DMA_NodeTypeDef const *const pNode3)
{
  uint32_t ref = 0U;

  /* Check node 1 parameter */
  if (pNode1 != NULL)
  {
    ref = pNode1->NodeInfo & NODE_TYPE_MASK;
  }
  /* Check node 2 parameter */
  else if (pNode2 != NULL)
  {
    ref = pNode2->NodeInfo & NODE_TYPE_MASK;
  }
  /* Check node 3 parameter */
  else if (pNode3 != NULL)
  {
    ref = pNode3->NodeInfo & NODE_TYPE_MASK;
  }
  else
  {
    /* Prevent MISRA-C2012-Rule-15.7 */
  }

  /* Check node 2 parameter */
  if (pNode2 != NULL)
  {
    /* Check node type compatibility */
    if (ref != (pNode2->NodeInfo & NODE_TYPE_MASK))
    {
      return 2U;
    }
  }

  /* Check node 3 parameter */
  if (pNode3 != NULL)
  {
    /* Check node type compatibility */
    if (ref != (pNode3->NodeInfo & NODE_TYPE_MASK))
    {
      return 3U;
    }
  }

  return 0U;
}

/**
  * @brief  Check nodes types compatibility.
  * @param  pNode       : Pointer to a DMA_NodeTypeDef structure that contains linked-list node registers
  *                       configurations.
  * @param  cllr_mask   : Pointer to CLLR register mask value.
  * @param  cllr_offset : Pointer to CLLR register offset value.
  * @retval None.
  */
static void DMA_List_GetCLLRNodeInfo(DMA_NodeTypeDef const *const pNode,
                                     uint32_t *const cllr_mask,
                                     uint32_t *const cllr_offset)
{
  /* Check node type */
  if ((pNode->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
  {
    /* Update CLLR register mask value */
    if (cllr_mask != NULL)
    {
      *cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | DMA_CLLR_UDA | DMA_CLLR_UT3 |
                   DMA_CLLR_UB2 | DMA_CLLR_ULL;
    }

    /* Update CLLR register offset */
    if (cllr_offset != NULL)
    {
      *cllr_offset = NODE_CLLR_2D_DEFAULT_OFFSET;
    }
  }
  /* Update CLLR and register number for linear addressing node */
  else
  {
    /* Update CLLR register mask value */
    if (cllr_mask != NULL)
    {
      *cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | DMA_CLLR_UDA | DMA_CLLR_ULL;
    }

    /* Update CLLR register offset */
    if (cllr_offset != NULL)
    {
      *cllr_offset = NODE_CLLR_LINEAR_DEFAULT_OFFSET;
    }
  }
}

/**
  * @brief  Find node in queue.
  * @param  pQList   : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @param  pNode    : Pointer to a DMA_NodeTypeDef structure that contains linked-list node registers configurations.
  * @param  NodeInfo : Pointer to a DMA_NodeInQInfoTypeDef structure that contains node linked to queue information.
  * @retval Return 0 when node is found in selected queue, otherwise node is not found.
  */
static uint32_t DMA_List_FindNode(DMA_QListTypeDef const *const pQList,
                                  DMA_NodeTypeDef const *const pNode,
                                  DMA_NodeInQInfoTypeDef *const NodeInfo)
{
  uint32_t node_idx = 0U;
  uint32_t currentnode_address  = 0U;
  uint32_t previousnode_address  = 0U;
  uint32_t cllr_offset = NodeInfo->cllr_offset;

  /* Find last node in queue */
  if (pNode ==  NULL)
  {
    /* Check that previous node is linked to the selected queue */
    while (node_idx < pQList->NodeNumber)
    {
      /* Get head node address */
      if (node_idx == 0U)
      {
        currentnode_address = (uint32_t)pQList->Head & DMA_CLLR_LA;
      }
      /* Calculate nodes addresses */
      else
      {
        previousnode_address = currentnode_address;
        currentnode_address =
          ((DMA_NodeTypeDef *)(currentnode_address +
                               ((uint32_t)pQList->Head & DMA_CLBAR_LBA)))->LinkRegisters[cllr_offset] & DMA_CLLR_LA;
      }

      /* Increment node index */
      node_idx++;
    }
  }
  /* Find selected node node in queue */
  else
  {
    /* Check that previous node is linked to the selected queue */
    while ((node_idx < pQList->NodeNumber) && (currentnode_address != ((uint32_t)pNode & DMA_CLLR_LA)))
    {
      /* Get head node address */
      if (node_idx == 0U)
      {
        currentnode_address = (uint32_t)pQList->Head & DMA_CLLR_LA;
      }
      /* Calculate nodes addresses */
      else
      {
        previousnode_address = currentnode_address;
        currentnode_address =
          ((DMA_NodeTypeDef *)(currentnode_address +
                               ((uint32_t)pQList->Head & DMA_CLBAR_LBA)))->LinkRegisters[cllr_offset] & DMA_CLLR_LA;
      }

      /* Increment node index */
      node_idx++;
    }
  }

  /* Check stored address */
  if (pNode != NULL)
  {
    if (currentnode_address != ((uint32_t)pNode & DMA_CLLR_LA))
    {
      return 1U;
    }
  }

  /* Update current node position */
  NodeInfo->currentnode_pos = node_idx;

  /* Update previous node address */
  NodeInfo->previousnode_addr = previousnode_address | ((uint32_t)pQList->Head & DMA_CLBAR_LBA);

  /* Update current node address */
  NodeInfo->currentnode_addr = currentnode_address | ((uint32_t)pQList->Head & DMA_CLBAR_LBA);

  /* Update next node address */
  if (((DMA_NodeTypeDef *)NodeInfo->currentnode_addr)->LinkRegisters[cllr_offset] != 0U)
  {
    NodeInfo->nextnode_addr = (((DMA_NodeTypeDef *)NodeInfo->currentnode_addr)->LinkRegisters[cllr_offset] &
                               DMA_CLLR_LA) | ((uint32_t)pQList->Head & DMA_CLBAR_LBA);
  }

  return 0U;
}

/**
  * @brief  Reset queue nodes.
  * @param  pQList   : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @param  NodeInfo : Pointer to a DMA_NodeInQInfoTypeDef structure that contains node linked to queue information.
  * @retval None.
  */
static void DMA_List_ResetQueueNodes(DMA_QListTypeDef const *const pQList,
                                     DMA_NodeInQInfoTypeDef const *const NodeInfo)
{
  uint32_t node_idx = 0U;
  uint32_t currentnode_address  = 0U;
  uint32_t previousnode_address;
  uint32_t cllr_offset = NodeInfo->cllr_offset;

  /* Check that previous node is linked to the selected queue */
  while (node_idx < pQList->NodeNumber)
  {
    /* Get head node address */
    if (node_idx == 0U)
    {
      previousnode_address = (uint32_t)pQList->Head & DMA_CLLR_LA;
      currentnode_address  = (pQList->Head->LinkRegisters[cllr_offset] & DMA_CLLR_LA);
    }
    /* Calculate nodes addresses */
    else
    {
      previousnode_address = currentnode_address;
      currentnode_address =
        ((DMA_NodeTypeDef *)(currentnode_address +
                             ((uint32_t)pQList->Head & DMA_CLBAR_LBA)))->LinkRegisters[cllr_offset] & DMA_CLLR_LA;
    }

    /* Reset node */
    ((DMA_NodeTypeDef *)(previousnode_address +
                         ((uint32_t)pQList->Head & DMA_CLBAR_LBA)))->LinkRegisters[cllr_offset] = 0U;

    /* Increment node index */
    node_idx++;
  }
}

/**
  * @brief  Fill source node registers values by destination nodes registers values.
  * @param  pSrcNode  : Pointer to a DMA_NodeTypeDef structure that contains linked-list source node registers
  *                     configurations.
  * @param  pDestNode : Pointer to a DMA_NodeTypeDef structure that contains linked-list destination node registers
  *                     configurations.
  * @retval None.
  */
static void DMA_List_FillNode(DMA_NodeTypeDef const *const pSrcNode,
                              DMA_NodeTypeDef *const pDestNode)
{
  /* Repeat for all register nodes */
  for (uint32_t reg_idx = 0U; reg_idx < NODE_MAXIMUM_SIZE; reg_idx++)
  {
    pDestNode->LinkRegisters[reg_idx] = pSrcNode->LinkRegisters[reg_idx];
  }

  /* Fill node information */
  pDestNode->NodeInfo = pSrcNode->NodeInfo;
}

/**
  * @brief  Convert node to dynamic.
  * @param  ContextNodeAddr : The context node address.
  * @param  CurrentNodeAddr : The current node address to be converted.
  * @param  RegisterNumber  : The register number to be converted.
  * @retval None.
  */
static void DMA_List_ConvertNodeToDynamic(uint32_t ContextNodeAddr,
                                          uint32_t CurrentNodeAddr,
                                          uint32_t RegisterNumber)
{
  uint32_t currentnode_reg_counter = 0U;
  uint32_t contextnode_reg_counter = 0U;
  uint32_t cllr_idx = RegisterNumber - 1U;
  DMA_NodeTypeDef *context_node = (DMA_NodeTypeDef *)ContextNodeAddr;
  DMA_NodeTypeDef *current_node = (DMA_NodeTypeDef *)CurrentNodeAddr;
  uint32_t update_link[NODE_MAXIMUM_SIZE] = {DMA_CLLR_UT1, DMA_CLLR_UT2, DMA_CLLR_UB1, DMA_CLLR_USA,
                                             DMA_CLLR_UDA, DMA_CLLR_UT3, DMA_CLLR_UB2, DMA_CLLR_ULL
                                            };

  /* Update ULL position according to register number */
  update_link[cllr_idx] = update_link[NODE_MAXIMUM_SIZE - 1U];

  /* Repeat for all node registers */
  while (contextnode_reg_counter != RegisterNumber)
  {
    /* Check if register values are equal (exception for CSAR, CDAR and CLLR registers) */
    if ((context_node->LinkRegisters[contextnode_reg_counter]  ==
         current_node->LinkRegisters[currentnode_reg_counter]) &&
        (contextnode_reg_counter != NODE_CSAR_DEFAULT_OFFSET)  &&
        (contextnode_reg_counter != NODE_CDAR_DEFAULT_OFFSET)  &&
        (contextnode_reg_counter != (RegisterNumber - 1U)))
    {
      /* Format the node according to unused registers */
      DMA_List_FormatNode(current_node, currentnode_reg_counter, RegisterNumber, NODE_DYNAMIC_FORMAT);

      /* Update CLLR index */
      cllr_idx --;

      /* Update CLLR fields */
      current_node->LinkRegisters[cllr_idx] &= ~update_link[contextnode_reg_counter];
    }
    else
    {
      /* Update context node register fields with new values */
      context_node->LinkRegisters[contextnode_reg_counter] = current_node->LinkRegisters[currentnode_reg_counter];

      /* Update CLLR fields */
      current_node->LinkRegisters[cllr_idx] |= update_link[contextnode_reg_counter];

      /* Increment current node number register counter */
      currentnode_reg_counter++;
    }

    /* Increment context node number register counter */
    contextnode_reg_counter++;
  }

  /* Update node information */
  MODIFY_REG(current_node->NodeInfo, NODE_CLLR_IDX, ((currentnode_reg_counter - 1U) << NODE_CLLR_IDX_POS));

  /* Clear unused node fields */
  DMA_List_ClearUnusedFields(current_node, currentnode_reg_counter);
}

/**
  * @brief  Convert node to static.
  * @param  ContextNodeAddr : The context node address.
  * @param  CurrentNodeAddr : The current node address to be converted.
  * @param  RegisterNumber  : The register number to be converted.
  * @retval None.
  */
static void DMA_List_ConvertNodeToStatic(uint32_t ContextNodeAddr,
                                         uint32_t CurrentNodeAddr,
                                         uint32_t RegisterNumber)
{
  uint32_t contextnode_reg_counter = 0U;
  uint32_t cllr_idx;
  uint32_t cllr_mask;
  const DMA_NodeTypeDef *context_node = (DMA_NodeTypeDef *)ContextNodeAddr;
  DMA_NodeTypeDef *current_node = (DMA_NodeTypeDef *)CurrentNodeAddr;
  uint32_t update_link[NODE_MAXIMUM_SIZE] = {DMA_CLLR_UT1, DMA_CLLR_UT2, DMA_CLLR_UB1, DMA_CLLR_USA,
                                             DMA_CLLR_UDA, DMA_CLLR_UT3, DMA_CLLR_UB2, DMA_CLLR_ULL
                                            };

  /* Update ULL position according to register number */
  update_link[RegisterNumber - 1U] = update_link[NODE_MAXIMUM_SIZE - 1U];

  /* Get context node CLLR information */
  cllr_idx  = (context_node->NodeInfo & NODE_CLLR_IDX) >> NODE_CLLR_IDX_POS;
  cllr_mask = context_node->LinkRegisters[cllr_idx];

  /* Repeat for all node registers */
  while (contextnode_reg_counter != RegisterNumber)
  {
    /* Check if node field is dynamic */
    if ((cllr_mask & update_link[contextnode_reg_counter]) == 0U)
    {
      /* Format the node according to unused registers */
      DMA_List_FormatNode(current_node, contextnode_reg_counter, RegisterNumber, NODE_STATIC_FORMAT);

      /* Update node field */
      current_node->LinkRegisters[contextnode_reg_counter] = context_node->LinkRegisters[contextnode_reg_counter];
    }

    /* Increment context node number register counter */
    contextnode_reg_counter++;
  }

  /* Update node information */
  MODIFY_REG(current_node->NodeInfo, NODE_CLLR_IDX, ((RegisterNumber - 1U) << NODE_CLLR_IDX_POS));
}

/**
  * @brief  Format the node according to unused registers.
  * @param  pNode           : Pointer to a DMA_NodeTypeDef structure that contains linked-list node registers
  *                           configurations.
  * @param  RegisterIdx     : The first register index to be formatted.
  * @param  RegisterNumber  : The number of node registers.
  * @param  Format          : The format type.
  * @retval None.
  */
static void DMA_List_FormatNode(DMA_NodeTypeDef *const pNode,
                                uint32_t RegisterIdx,
                                uint32_t RegisterNumber,
                                uint32_t Format)
{
  if (Format == NODE_DYNAMIC_FORMAT)
  {
    /* Repeat for all registers to be formatted */
    for (uint32_t reg_idx = RegisterIdx; reg_idx < (RegisterNumber - 1U); reg_idx++)
    {
      pNode->LinkRegisters[reg_idx] = pNode->LinkRegisters[reg_idx + 1U];
    }
  }
  else
  {
    /* Repeat for all registers to be formatted */
    for (uint32_t reg_idx = (RegisterNumber - 2U); reg_idx > RegisterIdx; reg_idx--)
    {
      pNode->LinkRegisters[reg_idx] = pNode->LinkRegisters[reg_idx - 1U];
    }
  }
}

/**
  * @brief  Clear unused register fields.
  * @param  pNode            : Pointer to a DMA_NodeTypeDef structure that contains linked-list node registers
  *                            configurations.
  * @param  FirstUnusedField : The first unused field to be cleared.
  * @retval None.
  */
static void DMA_List_ClearUnusedFields(DMA_NodeTypeDef *const pNode,
                                       uint32_t FirstUnusedField)
{
  /* Repeat for all unused fields */
  for (uint32_t reg_idx = FirstUnusedField; reg_idx < NODE_MAXIMUM_SIZE; reg_idx++)
  {
    pNode->LinkRegisters[reg_idx] = 0U;
  }
}

/**
  * @brief  Update CLLR for all dynamic queue nodes.
  * @param  pQList :              Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @param  LastNode_IsCircular : The first circular node is the last queue node or not.
  * @retval None.
  */
static void DMA_List_UpdateDynamicQueueNodesCLLR(DMA_QListTypeDef const *const pQList,
                                                 uint32_t LastNode_IsCircular)
{
  uint32_t previous_cllr_offset;
  uint32_t current_cllr_offset = 0U;
  uint32_t previousnode_addr;
  uint32_t currentnode_addr = (uint32_t)pQList->Head;
  uint32_t cllr_mask;
  uint32_t node_idx = 0U;

  /*  Repeat for all register nodes */
  while (node_idx < pQList->NodeNumber)
  {
    /* Get head node address */
    if (node_idx == 0U)
    {
      /* Get current node information */
      current_cllr_offset = (((DMA_NodeTypeDef *)currentnode_addr)->NodeInfo & NODE_CLLR_IDX) >> NODE_CLLR_IDX_POS;
    }
    /* Calculate nodes addresses */
    else
    {
      /* Get previous node information */
      previousnode_addr = currentnode_addr;
      previous_cllr_offset = current_cllr_offset;

      /* Get current node information */
      currentnode_addr = (((DMA_NodeTypeDef *)(previousnode_addr))->LinkRegisters[previous_cllr_offset] & DMA_CLLR_LA) +
                         ((uint32_t)pQList->Head & DMA_CLBAR_LBA);
      current_cllr_offset = (((DMA_NodeTypeDef *)currentnode_addr)->NodeInfo & NODE_CLLR_IDX) >> NODE_CLLR_IDX_POS;

      /* Calculate CLLR register value to be updated */
      cllr_mask = (((DMA_NodeTypeDef *)currentnode_addr)->LinkRegisters[current_cllr_offset] & ~DMA_CLLR_LA) |
                  (((DMA_NodeTypeDef *)(previousnode_addr))->LinkRegisters[previous_cllr_offset] & DMA_CLLR_LA);

      /* Set new CLLR value to previous node */
      ((DMA_NodeTypeDef *)(previousnode_addr))->LinkRegisters[previous_cllr_offset] = cllr_mask;
    }

    /* Increment node index */
    node_idx++;
  }

  /* Check queue circularity */
  if (pQList->FirstCircularNode != 0U)
  {
    /* First circular queue is not last queue node */
    if (LastNode_IsCircular == 0U)
    {
      /* Get CLLR node information */
      DMA_List_GetCLLRNodeInfo(((DMA_NodeTypeDef *)currentnode_addr), &cllr_mask, NULL);

      /* Update CLLR register for last circular node */
      ((DMA_NodeTypeDef *)currentnode_addr)->LinkRegisters[current_cllr_offset] =
        ((uint32_t)pQList->Head & DMA_CLLR_LA) | cllr_mask;
    }
    /* First circular queue is last queue node */
    else
    {
      /* Disable CLLR updating */
      ((DMA_NodeTypeDef *)currentnode_addr)->LinkRegisters[current_cllr_offset] &= ~DMA_CLLR_ULL;
    }
  }
  else
  {
    /* Clear CLLR register for last node */
    ((DMA_NodeTypeDef *)currentnode_addr)->LinkRegisters[current_cllr_offset] = 0U;
  }
}

/**
  * @brief  Update CLLR for all static queue nodes.
  * @param  pQList    : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @param  operation : The operation type.
  * @retval None.
  */
static void DMA_List_UpdateStaticQueueNodesCLLR(DMA_QListTypeDef const *const pQList,
                                                uint32_t operation)
{
  uint32_t currentnode_addr = (uint32_t)pQList->Head;
  uint32_t current_cllr_offset = ((uint32_t)pQList->Head->NodeInfo & NODE_CLLR_IDX) >> NODE_CLLR_IDX_POS;
  uint32_t cllr_default_offset;
  uint32_t cllr_default_mask;
  uint32_t cllr_mask;
  uint32_t node_idx = 0U;

  /* Get CLLR node information */
  DMA_List_GetCLLRNodeInfo(pQList->Head, &cllr_default_mask, &cllr_default_offset);

  /*  Repeat for all register nodes (Bypass last queue node) */
  while (node_idx < pQList->NodeNumber)
  {
    if (operation == UPDATE_CLLR_POSITION)
    {
      /* Get CLLR value */
      cllr_mask = ((DMA_NodeTypeDef *)currentnode_addr)->LinkRegisters[current_cllr_offset];
    }
    else
    {
      /* Calculate CLLR value */
      cllr_mask = (((DMA_NodeTypeDef *)currentnode_addr)->LinkRegisters[current_cllr_offset] & DMA_CLLR_LA) |
                  cllr_default_mask;
    }

    /* Set new CLLR value to default position */
    if ((node_idx == (pQList->NodeNumber - 1U)) && (pQList->FirstCircularNode == NULL))
    {
      ((DMA_NodeTypeDef *)(currentnode_addr))->LinkRegisters[cllr_default_offset] = 0U;
    }
    else
    {
      ((DMA_NodeTypeDef *)(currentnode_addr))->LinkRegisters[cllr_default_offset] = cllr_mask;
    }

    /* Update current node address with next node address */
    currentnode_addr = (currentnode_addr & DMA_CLBAR_LBA) | (cllr_mask & DMA_CLLR_LA);

    /* Update current CLLR offset with next CLLR offset */
    current_cllr_offset = (((DMA_NodeTypeDef *)currentnode_addr)->NodeInfo & NODE_CLLR_IDX) >> NODE_CLLR_IDX_POS;

    /* Increment node index */
    node_idx++;
  }
}

/**
  * @brief  Clean linked-list queue variable.
  * @param  pQList    : Pointer to a DMA_QListTypeDef structure that contains queue information.
  * @retval None.
  */
static void DMA_List_CleanQueue(DMA_QListTypeDef *const pQList)
{
  /* Clear head node */
  pQList->Head = NULL;

  /* Clear first circular queue node */
  pQList->FirstCircularNode = NULL;

  /* Reset node number */
  pQList->NodeNumber = 0U;

  /* Reset queue state */
  pQList->State = HAL_DMA_QUEUE_STATE_RESET;

  /* Reset queue error code */
  pQList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

  /* Reset queue type */
  pQList->Type = QUEUE_TYPE_STATIC;
}
/**
  * @}
  */

#endif /* HAL_DMA_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */
