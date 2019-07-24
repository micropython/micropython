/*
 * Amazon FreeRTOS V1.4.6
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* Called from all the example projects to start tasks that demonstrate Amazon
 * FreeRTOS libraries.
 *
 * If the project was created using the AWS console then this file will have been
 * auto generated and only reference and start the demos that were selected in the
 * console.  If the project was obtained from a source control repository then this
 * file will reference all the available and the developer can selectively comment
 * in or out the demos to execute. */


#include "aws_demo_runner.h"

/* Demo declarations. */
/* extern void vStartDeviceDefenderDemo( void ); */
/* extern void vStartGreenGrassDiscoveryTask( void ); */
extern void vStartMQTTEchoDemo( void );
/* extern void vStartOTAUpdateDemoTask( void ); */
/* extern void vStartShadowDemoTasks( void ); */
/* extern void vStartSimpleTCPServerTasks( void ); */
/* extern void vStartSubpubDemoTasks( void ); */
/* extern void vStartTCPEchoClientTasks_SeparateTasks( void ); */
/* extern void vStartTCPEchoClientTasks_SingleTasks( void ); */

/*-----------------------------------------------------------*/

/**
 * @brief Runs demos in the system.
 */
void DEMO_RUNNER_RunDemos( void )
{
    /* vStartDeviceDefenderDemo(); */
    /* vStartGreenGrassDiscoveryTask(); */
    vStartMQTTEchoDemo();
    /* vStartOTAUpdateDemoTask(); */
    /* vStartShadowDemoTasks(); */
    /* vStartSimpleTCPServerTasks(); */
    /* vStartSubpubDemoTasks(); */
    /* vStartTCPEchoClientTasks_SeparateTasks(); */
    /* vStartTCPEchoClientTasks_SingleTasks(); */
}
