server.c使用multithread來實現多user連線的功能，  
其實我自己用不到這些功能，只是想實做一下多執行緒而已，  
讀作業系統時發現單純用flag和while來實作lock很不好(performance不好而且還是有可能產生race condition)，
之後應該會改成用mutex_lock。
