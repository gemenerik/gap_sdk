/*
 * Copyright (C) 2018 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CL_PMSIS_TYPES__H__
#define __CL_PMSIS_TYPES__H__

/**
 * @addtogroup FcClusterSync
 * @{
 */

/**@{*/

/** \struct pi_cluster_conf
 * \brief Cluster configuration structure.
 *
 * This structure is used to pass the desired cluster configuration to the
 * runtime when opening a cluster.
 */
struct pi_cluster_conf {
    // do not move this one, might be accessed in various hackish way
    pi_device_e device_type; /*!< Device type. */
    int id;                  /*!< Cluster ID, starting from 0. */
    void *heap_start;        /* Reserved for internal usage. */
    uint32_t heap_size; /* Reserved for internal usage. */
    struct pmsis_event_kernel_wrap* event_kernel; /* Reserved for internal
      usage. */
};

//!@}

/**
 * @}
 */


/// @cond IMPLEM

#ifndef CLUSTER_TASK_IMPLEM
#define CLUSTER_TASK_IMPLEM
#endif

#ifndef INLINE
#define INLINE
#endif

struct pi_cluster_task {
    // entry function and its argument(s)
    void (*entry)(void*);
    void *arg;
    // pointer to first stack, and size for each cores
    void *stacks;
    uint32_t stack_size;
    uint32_t slave_stack_size;
    // Number of cores to be activated
    int nb_cores;
    // callback called at task completion
    pi_task_t *completion_callback;
    int stack_allocated;
    // to implement a fifo
    struct pi_cluster_task *next;

    CLUSTER_TASK_IMPLEM;
};

struct pi_cl_team_task {
    void *stacks;
    uint32_t stack_size;
    int core_mask;
    int nb_cores;
};

// object for device specific api
typedef struct cluster_driver_api {
    int (*send_task)(struct pi_device *device, struct pi_cluster_task *cl_task);
    int (*send_task_async)(struct pi_device *device, struct pi_cluster_task *cl_task
            , struct pi_task *async_task);
    void (*wait_free)(struct pi_device *device);
    void (*wait_free_async)(struct pi_device *device, struct pi_task *async_task);
} cluster_driver_api_t;

#if defined(PMSIS_DRIVERS)
// object for cluster driver specific data
struct cluster_driver_data {
    // prepare a small fifo so that FC can pipeline tasks
    // --> need to be first for inline access
    struct pi_cluster_task *task_first;
    struct pi_cluster_task *task_last;
    // event kernel attached
    struct pmsis_event_kernel_wrap* event_kernel;
    // metadata
    int cluster_is_on;
    pmsis_mutex_t task_mutex;
    pmsis_mutex_t powerstate_mutex;
    spinlock_t fifo_access;
    void *heap_start;
    uint32_t heap_size;
};
#endif

/// @endcond

#endif
