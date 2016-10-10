:mod:`json` --- JSON encoder and decoder
========================================

.. module:: json
   :synopsis: JSON encoder and decoder.

JSON (JavaScript Object Notation) is a lightweight data interchange
format inspired by JavaScript object literal syntax.

----------------------------------------------


.. function:: json.dumps(obj)

   Serialize `obj` to a JSON formatted str using this conversion
   table.


.. function:: json.loads(s)

   Deserialize `s` (a str instance containing a JSON document) to a
   Python object using this conversion table.
