..
    Copyright 2020 NXP



.. _write-apdu-to-buf:

=================================================
 Write APDU to buffer
=================================================

Normally, when we create an APDU, we also perform transceive
operation and return the result to the application.
Optionally, the user may want to just create the APDU
and not perform the transceive operation.

This can be done by creating your own transaction function.
All ``SE05x`` APIs call a common transaction function in
which the complete APDU is created and then transmitted.

The following function can be used as reference.

.. literalinclude:: se05x_GetAPDUBuffer.c
   :language: c
   :start-after: /* doc:start:createapdubuf */
   :end-before: /* doc:end:createapdubuf */

Assign this function to session context and use as
following

.. literalinclude:: se05x_GetAPDUBuffer.c
   :language: c
   :dedent: 4
   :start-after: /* doc:start:use-write-apdu-buf */
   :end-before: /* doc:end:use-write-apdu-buf */


After this, whichever ``SE05x`` API you call with ``write_apdubufferctx``,
it will just create the APDU and write it to the buffer. No transceive operation
will be performed.

Building
=================================================

Build the project with the following configurations.

**se05x_GetAPDUBuffer**

- ``SCP = None``
- ``Project = se05x_GetAPDUBuffer``

Running
=================================================

On successful execution you can see the APDU buffer printed out::

  App   :INFO :Policy for ReadOnly File:
  App   :INFO :Locked = READ
  App   :INFO :Excluded = DELETE, WRITE.
  App   :INFO :pPolicyBuffer (Len=9)
        08 00 00 00    00 00 20 00    00
  App   :INFO :gTxBuffer (Len=38)
        80 01 06 00    21 11 09 08    00 00 00 00    00 20 00 00
        41 04 11 22    33 44 43 02    00 0A 44 0A    01 02 03 04
        05 06 07 08    09 0A
