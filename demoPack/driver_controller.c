#include <stdio.h>
#include <stdbool.h>

#define WORK_TIME 200
#define REST_TIME 500

typedef enum {
    CF_STATE_INIT,
    CF_STATE_LOCK,
    CF_STATE_UNLOCK,
    CF_STATE_REST
} CFState;

typedef enum {
    RQ_LOCK,
    RQ_UNLOCK,
    RQ_SNA
} Request;

CFState g_cf_state = CF_STATE_INIT;
bool g_cfm_locked = false;

bool is_door_closed(void)
{
    // return open or close status of the door
}

Request get_lock_unlock_request(void)
{
    // return request
}

int get_time(void)
{
    // return system time
}

bool is_timeout(int startTime, int timer)
{
    return get_time() - startTime >= timer; // return if wait for work or rest enough time
}

void handle_lock_unlock_request(bool isLocked)
{
    if (g_cf_state == CF_STATE_INIT || (g_cf_state == CF_STATE_REST && g_cfm_locked != isLocked)) {
        g_cf_state = isLocked ? CF_STATE_LOCK : CF_STATE_UNLOCK;
    }
}

void handle_request(Request request)
{
    switch (request) {
        case RQ_LOCK:
            handle_lock_unlock_request(true);
            break;

        case RQ_UNLOCK:
            handle_lock_unlock_request(false);
            break;

        default:
            // do nothing
            break;
    }
}

void driver_control(bool a_driver, bool b_driver)
{
    if (a_driver && !b_driver) {
        // lock operation
    } else if (!a_driver && b_driver) {
        // unlcok operation
    } else if (!a_driver && !b_driver) {
        // rest operation
    }
}

bool handle_driver_work(bool action, int *driveStartTime)
{
    driver_control(action, !action); // lock or unlock operation
    *driveStartTime = get_time();
    g_cfm_locked = (action == 1);
    return true;
}

void handle_driver_rest(int *restStartTime)
{
    driver_control(false, false); // driver's rest
    *restStartTime = get_time();
}

void cf_state_machine(void)
{
    Request rqController = get_lock_unlock_request();
    static int driveStartTime = 0;
    static int restStartTime = 0;
    static bool isDriverWork = false;
    switch (g_cf_state) {
        case CF_STATE_INIT:
            // initialize the state machine
            isDriverWork = false;
            g_cfm_locked = false;
            restStartTime = 0;
            handle_request(rqController);
            break;

        case CF_STATE_LOCK:
        case CF_STATE_UNLOCK:
            if (!isDriverWork) {
                if (g_cf_state == CF_STATE_LOCK && is_door_closed() == true) {
                    isDriverWork = handle_driver_work(true, &driveStartTime);
                } else if (g_cf_state == CF_STATE_UNLOCK) {
                    isDriverWork = handle_driver_work(false, &driveStartTime);
                }
            }

            if (isDriverWork && is_timeout(driveStartTime, WORK_TIME)) {
                // You have to stop the driver and start counting rest timehere in case func call cycle time out
                handle_driver_rest(&restStartTime);
                g_cf_state = CF_STATE_REST;
                isDriverWork = false;
            }
            break;

        case CF_STATE_REST:
            if (is_timeout(restStartTime, REST_TIME)) {
                handle_request(rqController);
            }
            break;
    
            default:
            // should never reach here
            break;
    }
}
