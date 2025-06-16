# Mesh traffic decoding library

This application demonstrates how to decrypt and decode Bluetooth Mesh
network traffic using the PSA Crypto library for decrypting and
authentication.

Briefly, an application needs to first do the following:

* Initialize PSA crypto before calling any decoder library code

* Initialize a decoder library context

* Set the initial set of cryptographic keys (network keys, application
  key, and device keys with their associated device addresses)

* Set the initial network IV index value

* Set any known label UUIDs

Then, it can listen for Mesh network traffic and do the following when
advertising data is received:

* Decrypt the received network PDU, inspect the network PDU headers,
  and inspect the friendship context (if the network PDU was encrypted
  with friendship credentials)

* Inspect the lower transport layer PDU headers; in case the lower
  transport layer PDU carries segmented data, request the library
  to reassemble segments into complete upper transport layer PDUs

    * The library will manage memory usage for pending reassemblies
      automatically, but will have an upper limit on how many
      parallel reassemblies are kept track of.

* In case of upper transport layer control messages, inspect the
  control message opcode and parameters

    * The library will automatically track new friendships as they are
      formed, by tracking friendship-related control messages, and it
      will generate friendship credentials for decrypting the messages
      for the new friendship; but the application controls when
      friendship credentials are cleaned up from memory (for example,
      when a Friend Clear Confirm message is seen, or when it is known
      out-of-band that a device has been powered off and will no
      longer maintain a friendship).

    * Example code on how to inspect transport layer control message
      contents is provided

* In case of upper transport layer access messages, decrypt the
  access layer message and inspect the model message opcode and
  parameters

    * The library will not automatically track key management
      in the network. It is application's responsibility to
      add new encryption keys that are harvested from decoded
      control message payloads, or to clean up old encryption
      keys that are no longer in use.

    * Example code on how to inspect access message contents is
      not provided at the moment
 

