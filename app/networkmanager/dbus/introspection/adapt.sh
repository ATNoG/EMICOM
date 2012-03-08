#!/bin/sh

for FILE in *; do
	dbusxx-xml2cpp "${FILE}" --proxy=/dev/null --adaptor=../adaptors/"${FILE%.xml}".hpp;
done
