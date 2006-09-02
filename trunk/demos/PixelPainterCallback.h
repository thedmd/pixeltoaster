#ifndef PIXELTOASTER_CALLBACK_H
#define PIXELTOASTER_CALLBACK_H

namespace impl
{
	class Dispatcher0
	{
	public:
		virtual ~Dispatcher0() {}
		virtual void call() const { doCall(); }
		virtual Dispatcher0* clone() const { return doClone(); }
	private:
		virtual void doCall() const = 0;
		virtual Dispatcher0* doClone() const = 0;
	};
	
	template <typename Fun>
	class Dispatcher0Fun: public Dispatcher0
	{
	public:
		Dispatcher0Fun(Fun fun): fun_(fun) {}
	private:
		void doCall() const { fun_(); }
		Dispatcher0* doClone() const { return new Dispatcher0Fun<Fun>(fun_); }
		Fun fun_;
	};
	
	template <typename ObjPtr, typename Meth>
	class Dispatcher0Meth: public Dispatcher0
	{
	public:
		Dispatcher0Meth(ObjPtr obj, Meth meth): obj_(obj), meth_(meth) {}
	private:
		void doCall() const { (obj_->*meth_)(); }
		Dispatcher0* doClone() const { return new Dispatcher0Meth<ObjPtr, Meth>(obj_, meth_); }
		ObjPtr obj_;
		Meth meth_;
	};

	template <typename P>
	class Dispatcher1
	{
	public:
		virtual ~Dispatcher1() {}
		void call(P p) const { return doCall(p); }
		Dispatcher1<P>* clone() const { return doClone(); }
	private:
		virtual void doCall(P p) const = 0;
		virtual Dispatcher1<P>* doClone() const = 0;
	};
	
	template <typename P, typename Fun>
	class Dispatcher1Fun: public Dispatcher1<P>
	{
	public:
		Dispatcher1Fun(Fun fun): fun_(fun) {}
	private:
		void doCall(P p) const { fun_(p); }
		Dispatcher1<P>* doClone() const { return new Dispatcher1Fun<P, Fun>(fun_); }
		Fun fun_;
	};
	
	template <typename P, typename ObjPtr, typename Meth>
	class Dispatcher1Meth: public Dispatcher1<P>
	{
	public:
		Dispatcher1Meth(ObjPtr obj, Meth meth): obj_(obj), meth_(meth) {}
	private:
		void doCall(P p) const { (obj_->*meth_)(p); }
		Dispatcher1<P>* doClone() const { return new Dispatcher1Meth<P, ObjPtr, Meth>(obj_, meth_); }
		ObjPtr obj_;
		Meth meth_;
	};
}
	
class Callback0
{
public:
	Callback0(): disp_(0) {}
	template <typename Fun> 
	explicit Callback0(Fun fun): disp_(new impl::Dispatcher0Fun<Fun>(fun)) {}
	template <typename ObjPtr, typename Meth> 
	Callback0(ObjPtr obj, Meth meth): disp_(new impl::Dispatcher0Meth<ObjPtr, Meth>(obj, meth)) {}
	Callback0(const Callback0& other): disp_(other.disp_ ? other.disp_->clone() : 0) {}
	~Callback0() { delete disp_; }
	Callback0& operator=(Callback0 other) { return swap(other); }
	void operator()() const { if (disp_) disp_->call(); }
	Callback0& swap(Callback0& other) { std::swap(disp_, other.disp_); return *this; }
private:
	impl::Dispatcher0* disp_;
};
	
template <typename P>
class Callback1
{
public:
	Callback1(): disp_(0) {}
	template <typename Fun> 
	explicit Callback1(Fun fun): disp_(new impl::Dispatcher1Fun<P, Fun>(fun)) {}
	template <typename ObjPtr, typename Meth> 
	Callback1(ObjPtr obj, Meth meth): disp_(new impl::Dispatcher1Meth<P, ObjPtr, Meth>(obj, meth)) {}
	Callback1(const Callback1<P>& other): disp_(other.disp_ ? other.disp_->clone() : 0) {}
	~Callback1() { delete disp_; }
	Callback1<P>& operator=(Callback1<P> other) { return swap(other); }
	void operator()(P p) const { if (disp_) disp_->call(p); }
	Callback1<P>& swap(Callback1& other) { std::swap(disp_, other.disp_); return *this; }
private:
	impl::Dispatcher1<P>* disp_;
};



inline Callback0 makeCallback(void (*fun)())
{
	return Callback0(fun);
}

template <typename Obj> inline Callback0 makeCallback(Obj* obj, void (Obj::*meth)())
{
	return Callback0(obj, meth);
}

template <typename P> inline Callback1<P> makeCallback(void (*fun)(P))
{
	return Callback1<P>(fun);
}

template <typename P, typename Obj> inline Callback1<P> makeCallback(Obj* obj, void (Obj::*meth)(P))
{
	return Callback1<P>(obj, meth);
}

#endif
