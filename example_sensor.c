/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Copyright (c) 2021 Cascoda Ltd
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
#ifndef DOXYGEN
// Force doxygen to document static inline
#define STATIC static
#endif

/**
 * @file
 *  Example code for Function Block LSSB
 *  Implements only data point 61: switch on/off
 *  This implementation is a sensor, e.g. transmits data
 */
/**
 * ## Application Design
 *
 * support functions:
 *
 * - app_init
 *   initializes the stack values.
 * - register_resources
 *   function that registers all endpoints,
 *   e.g. sets the GET/PUT/POST/DELETE
 *      handlers for each end point
 *
 * - main
 *   starts the stack, with the registered resources.
 *   can be compiled out with NO_MAIN
 *
 *  handlers for the implemented methods (get/post):
 *   - get_[path]
 *     function that is being called when a GET is called on [path]
 *     set the global variables in the output
 *   - post_[path]
 *     function that is being called when a POST is called on [path]
 *     checks the input data
 *     if input data is correct
 *       updates the global variables
 *
 * ## stack specific defines
 *
 *  - OC_SECURITY
      enable security
 *    - OC_PKI
 *      enable use of PKI
 * - __linux__
 *   build for linux
 * - WIN32
 *   build for windows
 *
 * ## File specific defines
 *
 * - NO_MAIN
 *   compile out the function main()
 * - INCLUDE_EXTERNAL
 *   includes header file "external_header.h", so that other tools/dependencies
 can be included without changing this code
 */

#include "oc_api.h"
#include "oc_core_res.h"
#include "port/oc_clock.h"
#include <signal.h>
#include <stdlib.h>

#ifdef INCLUDE_EXTERNAL
/* import external definitions from header file*/
/* this file should be externally supplied */
#include "external_header.h"
#endif

#ifdef __linux__
/** linux specific code */
#include <pthread.h>
#ifndef NO_MAIN
static pthread_mutex_t mutex;
static pthread_cond_t cv;
static struct timespec ts;
#endif /* NO_MAIN */
#endif

#include <stdio.h> /* defines FILENAME_MAX */

#define MY_NAME "Sensor (LSSB) 421.61" /**< The name of the application */

#ifdef WIN32
/** windows specific code */
#include <windows.h>
STATIC CONDITION_VARIABLE cv; /**< event loop variable */
STATIC CRITICAL_SECTION cs;   /**< event loop variable */
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#define btoa(x) ((x) ? "true" : "false")

/** the state of the dpa 421.61 */
bool g_mystate = false;
volatile int quit = 0; /**< stop variable, used by handle_signal */

void
oc_add_s_mode_response_cb(char *url, oc_rep_t *rep, oc_rep_t *rep_value)
{
  PRINT("oc_add_s_mode_response_cb %s\n", url);
}

/**
 * function to set up the device.
 *
 * sets the:
 * - serial number
 * - friendly device name
 * - spec version
 *
 */
int
app_init(void)
{
  int ret = oc_init_platform("Cascoda", NULL, NULL);

  /* set the application name, version, base url, device serial number */
  ret |= oc_add_device(MY_NAME, "1.0", "//", "000003", NULL, NULL);

  oc_device_info_t *device = oc_core_get_device_info(0);
  PRINT("Serial Number: %s\n", oc_string(device->serialnumber));

  /* set the hardware version 1.0.0 */
  oc_core_set_device_hwv(0, 1, 0, 0);

  /* set the firmware version 1.0.0 */
  oc_core_set_device_fwv(0, 1, 0, 0);

  /* set the hardware type*/
  oc_core_set_device_hwt(0, "Pi");

  /* set the programming mode */
  oc_core_set_device_pm(0, true);

  /* set the model */
  oc_core_set_device_model(0, "my model");

  oc_set_s_mode_response_cb(oc_add_s_mode_response_cb);

  return ret;
}

/**
 * get method for "/p/o_1_1" resource.
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. Resource Description: This Resource describes a binary switch
 * (on/off). The Property "value" is a boolean. A value of 'true' means that the
 * switch is on. A value of 'false' means that the switch is off.
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_o_1_1(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* TODO: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /**< the error state, the generated code */
  int oc_status_code = OC_STATUS_OK;

  PRINT("-- Begin get_dpa_421_61: interface %d\n", interfaces);
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }

  CborError error;
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_mystate);
  oc_rep_end_root_object();
  
  if (error) {
    oc_status_code = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());

  if (error_state == false) {
    oc_send_cbor_response(request, oc_status_code);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_dpa_421_61\n");
}

/**
 * put method for "/p/o_1_1" resource.
 * The function has as input the request body, which are the input values of the
 * PUT method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values.
 * Resource Description:
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
void
put_o_1_1(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin put_dpa_421_61:\n");
  oc_rep_t *rep = NULL;

  if (oc_is_redirected_request(request)) {
    PRINT("  S-MODE or /p\n");
  }
  rep = request->request_payload;
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_BOOL)) {
      PRINT("  put_dpa_421_61 received : %d\n", rep->value.boolean);
      g_mystate = rep->value.boolean;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      PRINT("-- End put_dpa_421_61\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End put_dpa_421_61\n");
}

/**
 * register all the resources to the stack
 * this function registers all application level resources:
 * - each resource path is bind to a specific function for the supported methods
 * (GET, POST, PUT)
 * - each resource is
 *   - secure
 *   - observable
 *   - discoverable
 *   - used interfaces
 
 * URL Table
 * | resource url |  functional block/dpa  | GET | PUT |
 * | ------------ | ---------------------- | ----| ---- |
 * | /p/o_1_1     | urn:knx:dpa.421.61     | Yes | Yes  |
 */
void
register_resources(void)
{
  PRINT("Register Resource with local path \"/p/o_1_1\"\n");

  PRINT("Light Switching Sensor 421.61 (LSSB) : SwitchOnOff \n");
  PRINT("Data point 61 (DPT_Switch) \n");

  PRINT("Register Resource with local path \"/p/o_1_1\"\n");

  oc_resource_t *res_pushbutton =
    oc_new_resource("push button", "/p/o_1_1", 2, 0);
  oc_resource_bind_resource_type(res_pushbutton, "urn:knx:dpa.421.61");
  oc_resource_bind_resource_type(res_pushbutton, "DPT_Switch");
  oc_resource_bind_content_type(res_pushbutton, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_pushbutton, OC_IF_S); /* if.s */
  oc_resource_set_discoverable(res_pushbutton, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  // oc_resource_set_periodic_observable(res_pushbutton, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_352, true); */
  oc_resource_set_request_handler(res_pushbutton, OC_GET, get_o_1_1, NULL);
  oc_resource_set_request_handler(res_pushbutton, OC_PUT, put_o_1_1,
                                  NULL);
  oc_add_resource(res_pushbutton);
}

/**
 * initiate preset for device
 *
 * @param device the device identifier of the list of devices
 * @param data the supplied data.
 */
void
factory_presets_cb(size_t device, void *data)
{
  (void)device;
  (void)data;
}

/**
 * initializes the global variables
 * registers and starts the handler
 */
void
initialize_variables(void)
{
  /* initialize global variables for resources */
}

/* send a multicast s-mode message */
static void
issue_requests_s_mode(void)
{

  PRINT("Demo for interaction\n\n");

  oc_do_s_mode_with_scope(2, "/p/o_1_1", "w");
  oc_do_s_mode_with_scope(5, "/p/o_1_1", "w");
}

#ifndef NO_MAIN
#ifdef WIN32
/**
 * signal the event loop (windows version)
 * wakes up the main function to handle the next callback
 */
STATIC void
signal_event_loop(void)
{
  WakeConditionVariable(&cv);
}
#endif /* WIN32 */

#ifdef __linux__
/**
 * signal the event loop (Linux)
 * wakes up the main function to handle the next callback
 */
STATIC void
signal_event_loop(void)
{
  pthread_mutex_lock(&mutex);
  pthread_cond_signal(&cv);
  pthread_mutex_unlock(&mutex);
}
#endif /* __linux__ */

/**
 * handle Ctrl-C
 * @param signal the captured signal
 */
void
handle_signal(int signal)
{
  (void)signal;
  signal_event_loop();
  quit = 1;
}

#ifdef OC_SECURITY
/**
 * oc_ownership_status_cb callback implementation
 * handler to print out the DI after onboarding
 *
 * @param device_uuid the device ID
 * @param device_index the index in the list of device IDs
 * @param owned owned or unowned indication
 * @param user_data the supplied user data.
 */
void
oc_ownership_status_cb(const oc_uuid_t *device_uuid, size_t device_index,
                       bool owned, void *user_data)
{
  (void)user_data;
  (void)device_index;
  (void)owned;

  char uuid[37] = { 0 };
  oc_uuid_to_str(device_uuid, uuid, OC_UUID_LEN);
  PRINT(" oc_ownership_status_cb: DI: '%s'\n", uuid);
}
#endif /* OC_SECURITY */

void
print_usage()
{
  PRINT("Usage:\n");
  PRINT("no arguments : starts the server\n");
  PRINT("-help : this message\n");
  PRINT("s-mode: starts the server and issue a s-mode message at start up "
        "according the application/config\n");
  exit(0);
}

/**
 * main application.
 *       * initializes the global variables
 * registers and starts the handler
 *       * handles (in a loop) the next event.
 * shuts down the stack
 */
int
main(int argc, char *argv[])
{
  int init;
  oc_clock_time_t next_event;
  bool do_send_s_mode = false;

#ifdef WIN32
  /* windows specific */
  InitializeCriticalSection(&cs);
  InitializeConditionVariable(&cv);
  /* install Ctrl-C */
  signal(SIGINT, handle_signal);
#endif
#ifdef __linux__
  /* Linux specific */
  struct sigaction sa;
  sigfillset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handle_signal;
  /* install Ctrl-C */
  sigaction(SIGINT, &sa, NULL);
#endif

  for (int i = 0; i < argc; i++) {
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  if (argc > 1) {
    PRINT("s-mode: %s\n", argv[1]);
    if (strcmp(argv[1], "s-mode") == 0) {
      do_send_s_mode = true;
    }
    if (strcmp(argv[1], "-help") == 0) {
      print_usage();
    }
  }

  PRINT("KNX-IOT Server name : \"%s\"\n", MY_NAME);

  // show the current working folder
  char buff[FILENAME_MAX];
  char *retbuf = NULL;
  retbuf = GetCurrentDir(buff, FILENAME_MAX);
  if (retbuf != NULL) {
    PRINT("Current working dir: %s\n", buff);
  }

  /*
   The storage folder depends on the build system
   the folder is created in the makefile, with $target as name with _cred as
   post fix.
  */
  PRINT("\tstorage at './LSSB_minimal_all_creds' \n");
  oc_storage_config("./LSSB_minimal_all_creds");

  /*initialize the variables */
  initialize_variables();

  /* initializes the handlers structure */
  STATIC oc_handler_t handler = { .init = app_init,
                                  .signal_event_loop = signal_event_loop,
                                  .register_resources = register_resources,
                                  .requests_entry = NULL };

  if (do_send_s_mode) {

    handler.requests_entry = issue_requests_s_mode;
  }

  oc_set_factory_presets_cb(factory_presets_cb, NULL);

  /* start the stack */
  init = oc_main_init(&handler);

  if (init < 0) {
    PRINT("oc_main_init failed %d, exiting.\n", init);
    return init;
  }

#ifdef OC_SECURITY
  PRINT("Security - Enabled\n");
#else
  PRINT("Security - Disabled\n");
#endif /* OC_SECURITY */

  oc_device_info_t *device = oc_core_get_device_info(0);
  PRINT("serial number: %s", oc_string(device->serialnumber));

  PRINT("Server \"%s\" running, waiting on incoming "
        "connections.\n",
        MY_NAME);

#ifdef WIN32
  /* windows specific loop */
  while (quit != 1) {
    next_event = oc_main_poll();
    if (next_event == 0) {
      SleepConditionVariableCS(&cv, &cs, INFINITE);
    } else {
      oc_clock_time_t now = oc_clock_time();
      if (now < next_event) {
        SleepConditionVariableCS(
          &cv, &cs, (DWORD)((next_event - now) * 1000 / OC_CLOCK_SECOND));
      }
    }
  }
#endif

#ifdef __linux__
  /* Linux specific loop */
  while (quit != 1) {
    next_event = oc_main_poll();
    pthread_mutex_lock(&mutex);
    if (next_event == 0) {
      pthread_cond_wait(&cv, &mutex);
    } else {
      ts.tv_sec = (next_event / OC_CLOCK_SECOND);
      ts.tv_nsec = (next_event % OC_CLOCK_SECOND) * 1.e09 / OC_CLOCK_SECOND;
      pthread_cond_timedwait(&cv, &mutex, &ts);
    }
    pthread_mutex_unlock(&mutex);
  }
#endif

  /* shut down the stack */

  oc_main_shutdown();
  return 0;
}
#endif /* NO_MAIN */
