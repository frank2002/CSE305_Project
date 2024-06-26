---
c: Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
SPDX-License-Identifier: curl
Long: ftp-method
Arg: <method>
Help: Control CWD usage
Protocols: FTP
Added: 7.15.1
Category: ftp
Multi: single
See-also:
  - list-only
Example:
  - --ftp-method multicwd ftp://example.com/dir1/dir2/file
  - --ftp-method nocwd ftp://example.com/dir1/dir2/file
  - --ftp-method singlecwd ftp://example.com/dir1/dir2/file
---

# `--ftp-method`

Control what method curl should use to reach a file on an FTP(S)
server. The method argument should be one of the following alternatives:

## multicwd
Do a single CWD operation for each path part in the given URL. For deep
hierarchies this means many commands. This is how RFC 1738 says it should be
done. This is the default but the slowest behavior.

## nocwd
Do no CWD at all. curl does SIZE, RETR, STOR etc and gives the full path to
the server for each of these commands. This is the fastest behavior.

## singlecwd
Do one CWD with the full target directory and then operate on the file
"normally" (like in the multicwd case). This is somewhat more standards
compliant than `nocwd` but without the full penalty of `multicwd`.
