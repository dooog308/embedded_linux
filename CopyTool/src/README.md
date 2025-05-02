server.c使用multithread來實現多user連線的功能，  
其實我自己用不到這些功能，只是想實做一下多執行緒而已，  
server.c不使用mutex_lock和spin_lock是因為我這個程式  
的thread比較少，所以用busy wating跟用spin或mutex差異不大。
