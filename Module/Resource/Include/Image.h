#pragma once

struct ID2D1DeviceContext2;

namespace Resource
{
	class Image
	{
	public:
		Image(BYTE* binary, DWORD binarySize);
		~Image() = default;

		DLLEXPORT operator ID2D1Bitmap*() const;

		void SetD2DBitmap(ID2D1Bitmap* bitmap);

		std::pair<BYTE*, DWORD> GetBinary() const;
		ID2D1Bitmap* GetD2DBitmap() const;
		FLOAT2 GetSize() const;

	public:
		std::unique_ptr<BYTE[]> m_binary;
		DWORD m_binarySize;
		ComPtr<ID2D1Bitmap> m_d2dBitmap;
		ComPtr<ID3D12Resource> m_d3dResource;
	};

	DLLEXPORT FLOAT2 GetSize(Image* image);
	DLLEXPORT void UseAsD2D(const ComPtr<ID2D1DeviceContext2>& ctx, Image* image);
}
