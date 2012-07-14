#!/bin/sh

for FILE in *.xml; do
	dbusxx-xml2cpp "${FILE}" --proxy=../proxies/"${FILE%.xml}".hpp --adaptor=/dev/null ;
done
