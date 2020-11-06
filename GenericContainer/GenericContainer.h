// Default key definition for containers containing only one instance of data type
struct DefaultSlotKey;

template <class T, class Key = DefaultSlotKey>
class Slot
{
protected:
    T &doGet()
    {
        return m_value;
    }

    void doSet(const T &value)
    {
        m_value = value;
    }

private:
    T m_value;  
};

template <class... Slots>
class GenericContainer : private Slots...
{
public:
    template <class T, class Key = DefaultSlotKey>
    T &get()
    {
        return Slot<T, Key>::doGet();
    }

    template <class T, class Key = DefaultSlotKey>
    void set(const T &value)
    {
        Slot<T, Key>::doSet(value);
    }
};