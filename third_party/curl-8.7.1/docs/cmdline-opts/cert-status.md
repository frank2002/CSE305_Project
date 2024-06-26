---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Long: cert-status
Protocols: TLS
Added: 7.41.0
Help: Verify server cert status OCSP-staple
Category: tls
Multi: boolean
See-also:
  - pinnedpubkey
Example:
  - --cert-status $URL
---

# `--cert-status`

Verify the status of the server certificate by using the Certificate Status
Request (aka. OCSP stapling) TLS extension.

If this option is enabled and the server sends an invalid (e.g. expired)
response, if the response suggests that the server certificate has been
revoked, or no response at all is received, the verification fails.

This support is currently only implemented in the OpenSSL and GnuTLS backends.
