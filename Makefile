export V ?= 0

OUTPUT_DIR := $(CURDIR)/out

FEATURE_LIST := $(subst /,,$(dir $(wildcard */Makefile)))

.PHONY: all
all: features prepare-for-rootfs

.PHONY: clean
clean: features-clean prepare-for-rootfs-clean

features:
	@for feature in $(FEATURE_LIST); do \
		$(MAKE) -C $$feature CROSS_COMPILE="$(CROSS_COMPILE)" || exit 1; \
	done

features-clean:
	@for feature in $(FEATURE_LIST); do \
		$(MAKE) -C $$feature clean || exit 1; \
	done

prepare-for-rootfs: features
	@echo "Copying features CA and TA binaries to $(OUTPUT_DIR)..."
	@mkdir -p $(OUTPUT_DIR)
	@mkdir -p $(OUTPUT_DIR)/ta
	@mkdir -p $(OUTPUT_DIR)/ca
	@mkdir -p $(OUTPUT_DIR)/scripts
	@for feature in $(FEATURE_LIST); do \
		if [ -e $$feature/host/$$feature ]; then \
			cp -p $$feature/host/$$feature $(OUTPUT_DIR)/ca/; \
		fi; \
		if [ -e $$feature/ta/*.ta ]; then \
			cp -pr $$feature/ta/*.ta $(OUTPUT_DIR)/ta/; \
		fi; \
		if [ -e $$feature/scripts ]; then \
			cp -pr $$feature/scripts/* $(OUTPUT_DIR)/scripts/; \
		fi; \
	done

prepare-for-rootfs-clean:
	@rm -rf $(OUTPUT_DIR)/ta
	@rm -rf $(OUTPUT_DIR)/ca
	@rm -rf $(OUTPUT_DIR)/scripts
	@test ! -e $(OUTPUT_DIR) || rmdir --ignore-fail-on-non-empty $(OUTPUT_DIR)
