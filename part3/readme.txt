1. 先make，make會將server、client都編譯出來，因為中間有用到openssl的函式庫，牽涉到RSA的加解密，所以會有一串跟crypto有關的指令
2. all中的openssl genrsa會生出一個1024 bits的RSA private key，可以再用生出來的private key生成public key。
3. make clear會清除所有的東西，包括RSA密碼
4. 我的code裡面是讓client間、client與server間的訊息都用接收者的public key加密之後，再讓接收者用private key解密，避免被竊聽的可能。
