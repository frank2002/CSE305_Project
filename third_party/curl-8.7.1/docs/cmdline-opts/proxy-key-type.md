---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Long: proxy-key-type
Arg: <type>
Help: Private key file type for proxy
Added: 7.52.0
Category: proxy tls
Multi: single
See-also:
  - proxy-key
  - proxy
Example:
  - --proxy-key-type DER --proxy-key here -x https://proxy $URL
---

# `--proxy-key-type`

Same as --key-type but used in HTTPS proxy context.
