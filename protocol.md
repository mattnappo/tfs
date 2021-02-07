# Brief protocol spec

"Address" is inclusive (so "1-16" means that there is a byte at 1 and at 16).

"Length" is in bytes.

## Requests and Responses

| Address          | Data      | Length         |
|------------------|-----------|----------------|
| 0                | req\_type | 1              |
| 17,18            | body\_len | 2              |
| 19-...           | body      | REQ\_BODY\_LEN |

TOTAL SIEZ: 1+16+2+REQ\_BODY\_LEN

## Responses

| Address          | Data      | Length         |
|------------------|-----------|----------------|
| 0                | res\_type | 1              |
| 1,2              | body\_len | 2              |
| 3-...            | body      | RES\_BODY\_LEN |

TOTAL SIZE: 1+2+RES\_BODY\_LEN

