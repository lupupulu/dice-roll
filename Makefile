CC ?= gcc
CFLAGS ?= -O2 -s
TARGET = roll

$(TARGET): main.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	- rm -f ./$(TARGET)

install: $(TARGET)
	install -Dm755 $(TARGET) $(DESTDIR)/usr/bin/$(TARGET)

uninstall:
	-rm -f $(DESTDIR)/usr/bin/$(TARGET)

.PHONY: clean install uninstall
