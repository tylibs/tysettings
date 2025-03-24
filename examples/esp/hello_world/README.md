# Nonsense Hello World Logger  

This is a **rather nonsense example** that logs `"Hello World"` to the console.  
Why? Because we can 🚀 —and because it demonstrates the basic usage of TinySettings.  

## Running the Example  

To run this masterpiece on a Linux host, simply use:  

```sh
make linux
```

## Code  

``` c
// 1. Define the events using in the state table
TB_EVENT_NAME(MAIN, HELLO_WORLD, "HelloWorld");
TB_STATE_NAME(MAIN, START, "Start");

// 2. Declare the actions using in the state table
static void onHelloWorld(const Event *aEvent);

// 3. Glue everything together
static const StateTableRow stateTable[] = {{
    .state = MAIN_STATE_START,                // only process the event, if the current state
                                              // match. TB_STATE_INITIAL is the default state.
                                              // use TB_STATE_ANY, if this should be ignored
    .event          = MAIN_EVENT_HELLO_WORLD, // process if the event matches
    .conditionCheck = NULL,                   // No condition function.  The action is always executed.
    .entryAction    = onHelloWorld,           // The function to call when the event occurs.
    .nextState      = NULL,                   // Keep the current state after the action.
    .exitAction     = NULL,                   // No exit function.
    .stop           = true                    // Stop processing after this entry (this is the default case)
}};

/*****************************************************************************/
// Implement the Actions
static void onHelloWorld(const Event *aEvent) { ESP_LOGI(TAG, "Action onHelloWorld() called from TinySettings"); }

void app_main()
{
    // Subscribe to the TinySettings using the state table. This registers the module
    // to receive and process events according to the defined rules.
    ESP_LOGI(TAG, "Subscribe module '%s' to TinySettings", TAG);
    tbSubscribe(TB_SUBSCRIBER(TAG, stateTable, TB_TABLE_ROW_COUNT(stateTable), MAIN_STATE_START));

    while (true)
    {
        ESP_LOGI(TAG, "Publish HELLO_WORLD event");
        // Publish the HELLO_WORLD event.  The TinySettings will then look up this
        // event in the state table and execute the corresponding action
        // (onHelloWorld in this case).  The NULL and 0 arguments indicate no data
        // is being sent with the event.    // publish an event to the TinySettings
        tbPublish(TB_EVENT(MAIN_EVENT_HELLO_WORLD, NULL, 0));

        // next event in 1 second
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
```
