#pragma once

#ifdef __cplusplus

template<class TInterface>
class CComPtr {
private:
	TInterface *pointer;

public:
	void *operator new(std::size_t) = delete;
	void *operator new[](std::size_t) = delete;

	void operator delete(void *ptr) = delete;
	void operator delete[](void *ptr) = delete;

	CComPtr() {
		this->pointer = nullptr;
	}

	CComPtr(TInterface *lp) throw() {
		this->pointer = lp;
		if (this->pointer != NULL) {
			this->pointer->AddRef();
		}
	}

	~CComPtr() {
		if (this->pointer) {
			this->pointer->Release();
			this->pointer = nullptr;
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

	void Release() {
		this->~CComPtr();
	}

	TInterface *Detach() throw() {
		TInterface *ptr = this->pointer;
		this->pointer = nullptr;
		return ptr;
	}

	HRESULT CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL) throw() {
		return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, __uuidof(TInterface), (void **)&this->pointer);
	}

	HRESULT CoCreateInstance(REFCLSID rclsid, REFIID riid, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL) throw() {
		return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, (void **)&this->pointer);
	}
};

#endif
