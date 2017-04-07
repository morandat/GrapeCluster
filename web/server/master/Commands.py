class Commands():
    def __init__(self, orders_file_path):
        self.__orders = open(orders_file_path).read().split(";")

    def get_index(self, order_str):
        return str(self.__orders.index(order_str))

    def get_str(self, index):
        return self.__orders[index]

