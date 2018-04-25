#ifndef FUNCTION_HPP
#define FUNCTION_HPP

class Function
{
private:
	// Abstract interface for all callbacks
	class Callback
	{
	public:
		virtual ~Callback() { }
		virtual Callback* clone() const = 0;
		virtual void call() = 0;
	};
	
	// Implementation for member functions
	template <typename T>
	class ClassCallback : public Callback
	{
	private:
		T*      object;
		void (T::*callback)();
		
	public:
		ClassCallback(T* obj, void (T::*clbk)()) : object(obj), callback(clbk) {}
		virtual Callback* clone() const { return new ClassCallback<T>(object,callback); }
		virtual void call() { (object->*callback)(); }
	};
	
	// Implementation for global functions
	class GlobalCallback : public Callback
	{
	private:
		void (*callback)();
		
	public:
		GlobalCallback( void (*clbk)() ) : callback(clbk) {}
		virtual Callback* clone() const { return new GlobalCallback(callback); }
		virtual void call() { (*callback)(); }
	};
	
private:
	// Data member for the Function class
	Callback*       callback;
	
public:
	// basic construct/destruct
	Function() : callback(0) { }
	~Function() { delete callback; }
	
	// copy ctor/assign
	Function(const Function& rhs) : callback(0)
	{
		if(rhs.callback)    callback = rhs.callback->clone();
	}
	void operator = (const Function& rhs)
	{
		delete callback;
		if(rhs.callback)        callback = rhs.callback->clone();
		else                    callback = 0;
	}
	
	// construct with an actual callback
	Function(void (*clbk)())
	{
		callback = new GlobalCallback(clbk);
	}
	
	template <typename T>
	Function(T* obj, void (T::*clbk)())
	{
		callback = new ClassCallback<T>(obj,clbk);
	}
	
	// actually calling the function
	void operator () ()
	{
		callback->call();
	}
	
	bool operator == (Function f) {
		return callback == f.callback;
	}
};

#endif
