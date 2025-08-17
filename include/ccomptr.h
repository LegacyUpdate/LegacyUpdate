#pragma once

#ifdef __cplusplus

template<class TInterface>
class CComPtr {
public:
	TInterface *pointer;

	void *operator new(std::size_t) = delete;
	void *operator new[](std::size_t) = delete;

	void operator delete(void *ptr) = delete;
	void operator delete[](void *ptr) = delete;

	CComPtr() {
		this->pointer = NULL;
	}

	CComPtr(TInterface *ptr) throw() {
		this->pointer = ptr;
		if (this->pointer != NULL) {
			this->pointer->AddRef();
		}
	}

	CComPtr(CComPtr<TInterface> &other) : CComPtr(other.pointer) {}

	CComPtr(CComPtr<TInterface> &&other) throw() {
		other.Swap(*this);
	}

	~CComPtr() {
		TInterface *pointer = this->pointer;
		if (pointer != NULL) {
			this->pointer = NULL;
			pointer->Release();
		}
	}

	operator TInterface *() {
		return this->pointer;
	}

	operator TInterface *() const {
		return this->pointer;
	}

	TInterface &operator *() {
		return *this->pointer;
	}

	TInterface &operator *() const {
		return *this->pointer;
	}

	TInterface **operator &() {
		return &this->pointer;
	}

	TInterface **operator &() const {
		return &this->pointer;
	}

	TInterface *operator ->() {
		return this->pointer;
	}

	TInterface *operator ->() const {
		return this->pointer;
	}

	TInterface *operator =(TInterface *other) throw()
	{
		if (this->pointer != other) {
			CComPtr(other).Swap(*this);
		}
		return *this;
	}

	TInterface *operator =(const CComPtr<TInterface> &other) throw()
	{
		if (this->pointer != other.pointer) {
			CComPtr(other).Swap(*this);
		}
		return *this;
	}

	TInterface *operator =(CComPtr<TInterface> &&other) throw() {
		if (this->pointer != other.pointer) {
			CComPtr(static_cast<CComPtr&&>(other)).Swap(*this);
		}
		return *this;
	}

	bool operator !() const throw() {
		return this->pointer == NULL;
	}

	bool operator <(TInterface *other) const throw() {
		return this->pointer < other;
	}

	bool operator ==(TInterface *other) const throw() {
		return this->pointer == other;
	}

	bool operator ==(const CComPtr &other) const throw() {
		return this->pointer == other;
	}

	void Release() {
		this->~CComPtr();
	}

	TInterface *Detach() throw() {
		TInterface *ptr = this->pointer;
		this->pointer = NULL;
		return ptr;
	}

	void Swap(CComPtr &other) {
		TInterface *ptr = this->pointer;
		ptr = other.pointer;
		other.pointer = ptr;
	}

	HRESULT CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL) throw() {
		return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, __uuidof(TInterface), (void **)&this->pointer);
	}

	HRESULT CoCreateInstance(REFCLSID rclsid, REFIID riid, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL) throw() {
		return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, (void **)&this->pointer);
	}
};

#endif
