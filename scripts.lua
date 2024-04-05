--a = 1+2
--print(a)

print("put k/v to db, key: k1, value: v1")
PUT("k1", "v1")
print("-------")

print("get value from db, key: k1")
a = GET("k1")
print("value in db for key k1 is:", a)
print("-------")

print("delete key from db, key: k1")
DEL("k1")
print("-------")

print("get value from db, key: k1")
a = GET("k1")
print("value in db for key k1 is:", a)
print("=======")


