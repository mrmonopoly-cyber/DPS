#ifndef __DPS_SLAVE__
#define __DPS_SLAVE__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../common/common.h"

#if UINTPTR_MAX == 0xFFFFFFFF
#define DPS_SLAVE_SIZE 284
#define DPS_SLAVE_ALIGN 4
#else
#define DPS_SLAVE_SIZE 424
#define DPS_SLAVE_ALIGN 8
#endif

/**
 * @brief Function pointer type for post-update callbacks.
 *
 * This function is called when a monitored variable is updated by the master.
 *
 * @param var_name Name of the updated variable.
 * @param var Pointer to the updated variable data.
 * @return 0 on success, negative value on failure.
 */
typedef int8_t (*post_update)(const char* const var_name, const void* const var);

/**
 * @brief Structure representing a DPS slave instance.
 *
 * The internal state is encapsulated in private_data and should not be accessed directly.
 */
typedef struct __attribute__((aligned(DPS_SLAVE_ALIGN))){
    const uint8_t private_data[DPS_SLAVE_SIZE]; ///< Internal state (do not access directly)
} DpsSlave_h;

/**
 * @brief Initializes the DPS slave instance.
 *
 * @param self Pointer to the DPS slave object.
 * @param send_f Function pointer for sending CAN messages.
 * @param wait_f Function pointer for waiting after sending a mex.
 * @param board_name Null-terminated board name (must be BOARD_NAME_LENGTH chars or less).
 * @param dps_board_id Unique board ID (0-15).
 * @param dps_can_id_master CAN ID used by the master.
 * @param dps_can_id_slaves CAN ID used by slaves (must match system config).
 * @return 0 on success, negative value on error (e.g., null pointers, bad IDs).
 *
 * @pre `self`, `send_f`, and `board_name` must be non-null.
 * @pre `dps_board_id` must be less than 16.
 * @post The slave is ready to be started with `dps_slave_start`.
 */
int8_t
dps_slave_init(DpsSlave_h* const restrict self,
        const can_send send_f,
        const wait_after_send wait_f,
        const char board_name[BOARD_NAME_LENGTH],
        const uint8_t dps_board_id,
        const uint16_t dps_can_id_master,
        const uint16_t dps_can_id_slaves);

/**
 * @brief Starts the DPS slave to begin communication and variable updates.
 *
 * @param self Pointer to the DPS slave object.
 * @return 0 on success, negative on failure.
 *
 * @pre `self` must be initialized with `dps_slave_init`.
 * @post Slave will monitor and respond to CAN messages.
 */
int8_t
dps_slave_start(DpsSlave_h* const restrict self);

/**
 * @brief Disables the DPS slave, stopping variable monitoring and message processing.
 *
 * @param self Pointer to the DPS slave object.
 * @return 0 on success, negative on failure.
 *
 * @pre `self` must have been initialized.
 * @post The slave no longer responds to CAN commands.
 */
int8_t
dps_slave_disable(DpsSlave_h* const restrict self);

/**
 * @brief Registers a primitive variable to be monitored and remotely updated.
 *
 * @param self Pointer to the DPS slave object.
 * @param type Type of the primitive variable (see DPS_PRIMITIVE_TYPES).
 * @param p_data Pointer to the actual variable to monitor.
 * @param post_update_f Optional callback called after the variable is updated (can be NULL).
 * @param name Name of the variable (must be <= VAR_NAME_LENGTH).
 * @return 0 on success, negative on failure.
 *
 * @pre `self`, `p_data`, and `name` must be non-null.
 * @pre `type` must be a valid `DPS_PRIMITIVE_TYPES` value.
 * @pre `name` must be unique among monitored variables.
 * @post The variable will be updated based on commands from the master.
 */
int8_t
dps_monitor_primitive_var(DpsSlave_h* const restrict self,
        const enum DPS_PRIMITIVE_TYPES type,
        void* const p_data,
        post_update post_update_f,
        const char name[VAR_NAME_LENGTH]);

/**
 * @brief Processes an incoming CAN message and checks for valid DPS commands.
 *
 * @param self Pointer to the DPS slave object.
 * @param mex Pointer to the received CAN message.
 *
 * @pre `self` and `mex` must be non-null.
 * @post If the message is addressed to this board and valid, it may update a monitored variable.
 */
int8_t
dps_slave_check_can_command_recv(DpsSlave_h* const restrict self,
        const DpsCanMessage* const mex);

/**
 * @brief Cleans up resources used by the DPS slave.
 *
 * This should be called before deallocating the object (if applicable).
 *
 * @param self Pointer to the DPS slave object.
 * @post The object is no longer valid and should not be used.
 */
void
dps_slave_destroy(DpsSlave_h* const restrict self);


#ifdef DEBUG

/**
 * @brief Returns the board ID of the slave instance.
 *
 * @param self Pointer to the DPS slave object.
 * @return Board ID on success, negative on failure.
 */
int dps_get_id(const DpsSlave_h* const restrict self);

/**
 * @brief Prints information about all monitored variables.
 *
 * @param self Pointer to the DPS slave object.
 * @return 0 on success, negative on failure.
 */
int dps_print_var(const DpsSlave_h* const restrict self);

#endif // DEBUG

#ifdef __cplusplus
}
#endif

#endif // !__DPS_SLAVE__

