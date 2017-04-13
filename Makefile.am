SUBDIRS	= extradist docs \
	src 

EXTRA_DIST = TODO myproject.spec myproject.spec.in \
	am_rpm_init.m4 autogen.sh

ACLOCAL_AMFLAGS = -I m4

### possibly currently defunct rpm target
if MAKE_RPMS
rpm: @RPM_TARGET@
.PHONY: rpm
$(RPM_TARGET): $(DISTFILES)
	${MAKE} dist
	-mkdir -p $(RPM_DIR)/SRPMS
	-mkdir -p `dirname $(RPM_TARGET)`
	$(RPM_PROG) $(RPM_ARGS) $(RPM_TARBALL)
	@echo Congratulations, $(RPM_TARGET) "(and friends)" should now exist.
else
endif

### target list
list:
#.PHONY: list
	@$(MAKE) -pRrq -f $(lastword $(MAKEFILE_LIST)) : 2>/dev/null | awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | sort | egrep -v -e '^[^[:alnum:]]' -e '^$@$$' | xargs
