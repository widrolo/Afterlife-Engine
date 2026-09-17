#!/bin/bash

scriptDir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$scriptDir" || exit 1
exec ./Afterlife_Browser
