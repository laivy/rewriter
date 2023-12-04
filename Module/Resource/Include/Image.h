#pragma once
#include "Common/Types.h"

struct ID2D1DeviceContext2;

namespace Resource
{
	class Image
	{
	public:
		Image(BYTE* binary, DWORD binarySize);
		~Image() = default;

		__declspec(dllexport) operator ID2D1Bitmap*() const;

		void SetD2DBitmap(ID2D1Bitmap* bitmap);

		std::pair<BYTE*, DWORD> GetBinary() const;
		ID2D1Bitmap* GetD2DBitmap() const;
		INT2 GetSize() const;

	public:
		std::unique_ptr<BYTE[]> m_binary;
		DWORD m_binarySize;
		ComPtr<ID2D1Bitmap> m_d2dBitmap;
		ComPtr<ID3D12Resource> m_d3dResource;
	};

	// 이미지의 가로, 세로 크기를 반환
	__declspec(dllexport) INT2 GetSize(const std::shared_ptr<Resource::Image>& image);

	// 해당 이미지를 Direct2D 렌더링에 사용함
	__declspec(dllexport) void UseAsD2D(const ComPtr<ID2D1DeviceContext2>& ctx, const std::shared_ptr<Resource::Image>& image);
}
