# MESSAGE FORMATS

Sections of the message are delineated with colons.

## INCOMING MESSAGES

Some incoming messages can be single words:

- STOP
- STATUS

## OUTGOING MESSAGES

We create a dict for outgoing messages. This is sent using the `send_msg_dict(...)` function which forms the dict into separate lines (one for each key) for serial channels and as JSON for the WebSocket channel.

Standard keys that should be included in all messages:

- `type` - 'CMD' (default), 'CFG', 'MOT', 'STAT', 'SYS'
- `cmd` - repeats back the received command
- `msg` - will be an empty string for responses that don't need it
- `result` - 'ERR' or 'OK'

Other keys will depend on the data being sent.
