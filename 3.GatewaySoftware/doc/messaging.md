# Messaging

## MessageChannels

- AMQP
- HTTP REST

## MessageTypes

All messagetypes are defined in Google Protobuffer, so they can be used in many different programming languages

BaseMessage:
- String Sender
- String Receiver

The defined types are:

- RegisterModule
- DeregisterModule
- DeviceSignal
    * Reply: None
    * Extra fields: string devicecontroller, string deviceid, string data
- GetDevices
    * Reply: DeviceList
- DeviceList
- GetCommands
    * Reply: CommandList
- CommandList
- ExecuteCommand
    * Reply: BooleanReply
    * Extra fields: Command command,string commandparameters
- BooleanReply
    * Extra fields: boolean ok

