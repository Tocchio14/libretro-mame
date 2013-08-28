//============================================================
//
//  d3d9intf.c - Direct3D 9 abstraction layer
//
//============================================================
//
//  Copyright Aaron Giles
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or
//  without modification, are permitted provided that the
//  following conditions are met:
//
//    * Redistributions of source code must retain the above
//      copyright notice, this list of conditions and the
//      following disclaimer.
//    * Redistributions in binary form must reproduce the
//      above copyright notice, this list of conditions and
//      the following disclaimer in the documentation and/or
//      other materials provided with the distribution.
//    * Neither the name 'MAME' nor the names of its
//      contributors may be used to endorse or promote
//      products derived from this software without specific
//      prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY AARON GILES ''AS IS'' AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
//  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
//  EVENT SHALL AARON GILES BE LIABLE FOR ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  DAMAGE (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
//  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
//  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//============================================================

// standard windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>
#undef interface

// MAME headers
#include "emu.h"

// MAMEOS headers
#include "d3dintf.h"
#include "strconv.h"
#include "winmain.h"



//============================================================
//  TYPE DEFINITIONS
//============================================================

typedef IDirect3D9 *(WINAPI *direct3dcreate9_ptr)(UINT SDKVersion);

typedef HRESULT (WINAPI *direct3dx9_loadeffect_ptr)(LPDIRECT3DDEVICE9 pDevice, LPCTSTR pSrcFile, const D3DXMACRO *pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT *ppEffect, LPD3DXBUFFER *ppCompilationErrors);
static direct3dx9_loadeffect_ptr g_load_effect = NULL;


namespace d3d
{
//============================================================
//  PROTOTYPES
//============================================================

static void set_interfaces(base *d3dptr);

//============================================================
//  INLINES
//============================================================

INLINE void convert_present_params(const present_parameters *params, D3DPRESENT_PARAMETERS *d3d9params)
{
	memset(d3d9params, 0, sizeof(*d3d9params));
	d3d9params->BackBufferWidth = params->BackBufferWidth;
	d3d9params->BackBufferHeight = params->BackBufferHeight;
	d3d9params->BackBufferFormat = params->BackBufferFormat;
	d3d9params->BackBufferCount = params->BackBufferCount;
	d3d9params->MultiSampleType = params->MultiSampleType;
	d3d9params->MultiSampleQuality = params->MultiSampleQuality;
	d3d9params->SwapEffect = params->SwapEffect;
	d3d9params->hDeviceWindow = params->hDeviceWindow;
	d3d9params->Windowed = params->Windowed;
	d3d9params->EnableAutoDepthStencil = params->EnableAutoDepthStencil;
	d3d9params->AutoDepthStencilFormat = params->AutoDepthStencilFormat;
	d3d9params->Flags = params->Flags;
	d3d9params->FullScreen_RefreshRateInHz = params->FullScreen_RefreshRateInHz;
	d3d9params->PresentationInterval = params->PresentationInterval;
}



//============================================================
//  drawd3d9_init
//============================================================

base *drawd3d9_init(void)
{
	direct3dcreate9_ptr direct3dcreate9;
	HINSTANCE dllhandle;
	IDirect3D9 *d3d9;
	base *d3dptr;
	bool post_available = true;

	// dynamically grab the create function from d3d9.dll
	dllhandle = LoadLibrary(TEXT("d3d9.dll"));
	if (dllhandle == NULL)
	{
		mame_printf_verbose("Direct3D: Unable to access d3d9.dll\n");
		return NULL;
	}

	// import the create function
	direct3dcreate9 = (direct3dcreate9_ptr)GetProcAddress(dllhandle, "Direct3DCreate9");
	if (direct3dcreate9 == NULL)
	{
		mame_printf_verbose("Direct3D: Unable to find Direct3DCreate9\n");
		FreeLibrary(dllhandle);
		dllhandle = NULL;
		return NULL;
	}

	// create our core direct 3d object
	d3d9 = (*direct3dcreate9)(D3D_SDK_VERSION);
	if (d3d9 == NULL)
	{
		mame_printf_verbose("Direct3D: Error attempting to initialize Direct3D9\n");
		FreeLibrary(dllhandle);
		dllhandle = NULL;
		return NULL;
	}

	// dynamically grab the shader load function from d3dx9.dll
	HINSTANCE fxhandle = LoadLibrary(TEXT("d3dx9_43.dll"));
	if (fxhandle == NULL)
	{
		post_available = false;
		mame_printf_verbose("Direct3D: Warning - Unable to access d3dx9_43.dll; disabling post-effect rendering\n");
	}

	// import the create function
	if(post_available)
	{
		g_load_effect = (direct3dx9_loadeffect_ptr)GetProcAddress(fxhandle, "D3DXCreateEffectFromFileW");
		if (g_load_effect == NULL)
		{
			printf("Direct3D: Unable to find D3DXCreateEffectFromFileW\n");
			FreeLibrary(dllhandle);
			fxhandle = NULL;
			dllhandle = NULL;
			return NULL;
		}
	}
	else
	{
		g_load_effect = NULL;
		post_available = false;
		mame_printf_verbose("Direct3D: Warning - Unable to get a handle to D3DXCreateEffectFromFileW; disabling post-effect rendering\n");
	}

	// allocate an object to hold our data
	d3dptr = global_alloc(base);
	d3dptr->version = 9;
	d3dptr->d3dobj = d3d9;
	d3dptr->dllhandle = dllhandle;
	d3dptr->post_fx_available = post_available;
	set_interfaces(d3dptr);

	mame_printf_verbose("Direct3D: Using Direct3D 9\n");
	return d3dptr;
}



//============================================================
//  Direct3D interfaces
//============================================================

static HRESULT check_device_format(base *d3dptr, UINT adapter, D3DDEVTYPE devtype, D3DFORMAT adapterformat, DWORD usage, D3DRESOURCETYPE restype, D3DFORMAT format)
{
	IDirect3D9 *d3d9 = (IDirect3D9 *)d3dptr->d3dobj;
	return IDirect3D9_CheckDeviceFormat(d3d9, adapter, devtype, adapterformat, usage, restype, format);
}


static HRESULT check_device_type(base *d3dptr, UINT adapter, D3DDEVTYPE devtype, D3DFORMAT format, D3DFORMAT backformat, BOOL windowed)
{
	IDirect3D9 *d3d9 = (IDirect3D9 *)d3dptr->d3dobj;
	return IDirect3D9_CheckDeviceType(d3d9, adapter, devtype, format, backformat, windowed);
}

static HRESULT create_device(base *d3dptr, UINT adapter, D3DDEVTYPE devtype, HWND focus, DWORD behavior, present_parameters *params, device **dev)
{
	IDirect3D9 *d3d9 = (IDirect3D9 *)d3dptr->d3dobj;
	D3DPRESENT_PARAMETERS d3d9params;
	convert_present_params(params, &d3d9params);
	return IDirect3D9_CreateDevice(d3d9, adapter, devtype, focus, behavior, &d3d9params, (IDirect3DDevice9 **)dev);
}

static HRESULT enum_adapter_modes(base *d3dptr, UINT adapter, D3DFORMAT format, UINT index, D3DDISPLAYMODE *mode)
{
	IDirect3D9 *d3d9 = (IDirect3D9 *)d3dptr->d3dobj;
	return IDirect3D9_EnumAdapterModes(d3d9, adapter, format, index, mode);
}


static UINT get_adapter_count(base *d3dptr)
{
	IDirect3D9 *d3d9 = (IDirect3D9 *)d3dptr->d3dobj;
	return IDirect3D9_GetAdapterCount(d3d9);
}


static HRESULT get_adapter_display_mode(base *d3dptr, UINT adapter, D3DDISPLAYMODE *mode)
{
	IDirect3D9 *d3d9 = (IDirect3D9 *)d3dptr->d3dobj;
	return IDirect3D9_GetAdapterDisplayMode(d3d9, adapter, mode);
}


static HRESULT get_adapter_identifier(base *d3dptr, UINT adapter, DWORD flags, adapter_identifier *identifier)
{
	IDirect3D9 *d3d9 = (IDirect3D9 *)d3dptr->d3dobj;
	D3DADAPTER_IDENTIFIER9 id;
	HRESULT result = IDirect3D9_GetAdapterIdentifier(d3d9, adapter, flags, &id);
	memcpy(identifier->Driver, id.Driver, sizeof(identifier->Driver));
	memcpy(identifier->Description, id.Description, sizeof(identifier->Description));
	identifier->DriverVersion = id.DriverVersion;
	identifier->VendorId = id.VendorId;
	identifier->DeviceId = id.DeviceId;
	identifier->SubSysId = id.SubSysId;
	identifier->Revision = id.Revision;
	identifier->DeviceIdentifier = id.DeviceIdentifier;
	identifier->WHQLLevel = id.WHQLLevel;
	return result;
}


static UINT get_adapter_mode_count(base *d3dptr, UINT adapter, D3DFORMAT format)
{
	IDirect3D9 *d3d9 = (IDirect3D9 *)d3dptr->d3dobj;
	return IDirect3D9_GetAdapterModeCount(d3d9, adapter, format);
}


static HMONITOR get_adapter_monitor(base *d3dptr, UINT adapter)
{
	IDirect3D9 *d3d9 = (IDirect3D9 *)d3dptr->d3dobj;
	return IDirect3D9_GetAdapterMonitor(d3d9, adapter);
}


static HRESULT get_caps_dword(base *d3dptr, UINT adapter, D3DDEVTYPE devtype, caps_index which, DWORD *value)
{
	IDirect3D9 *d3d9 = (IDirect3D9 *)d3dptr->d3dobj;
	D3DCAPS9 caps;
	HRESULT result = IDirect3D9_GetDeviceCaps(d3d9, adapter, devtype, &caps);
	switch (which)
	{
		case CAPS_PRESENTATION_INTERVALS:   *value = caps.PresentationIntervals;    break;
		case CAPS_CAPS2:                    *value = caps.Caps2;                    break;
		case CAPS_DEV_CAPS:                 *value = caps.DevCaps;                  break;
		case CAPS_SRCBLEND_CAPS:            *value = caps.SrcBlendCaps;             break;
		case CAPS_DSTBLEND_CAPS:            *value = caps.DestBlendCaps;            break;
		case CAPS_TEXTURE_CAPS:             *value = caps.TextureCaps;              break;
		case CAPS_TEXTURE_FILTER_CAPS:      *value = caps.TextureFilterCaps;        break;
		case CAPS_TEXTURE_ADDRESS_CAPS:     *value = caps.TextureAddressCaps;       break;
		case CAPS_TEXTURE_OP_CAPS:          *value = caps.TextureOpCaps;            break;
		case CAPS_MAX_TEXTURE_ASPECT:       *value = caps.MaxTextureAspectRatio;    break;
		case CAPS_MAX_TEXTURE_WIDTH:        *value = caps.MaxTextureWidth;          break;
		case CAPS_MAX_TEXTURE_HEIGHT:       *value = caps.MaxTextureHeight;         break;
		case CAPS_STRETCH_RECT_FILTER:      *value = caps.StretchRectFilterCaps;    break;
		case CAPS_MAX_PS30_INSN_SLOTS:      *value = caps.MaxPixelShader30InstructionSlots; break;
	}
	return result;
}


static ULONG release(base *d3dptr)
{
	IDirect3D9 *d3d9 = (IDirect3D9 *)d3dptr->d3dobj;
	ULONG result = IDirect3D9_Release(d3d9);
	FreeLibrary(d3dptr->dllhandle);
	global_free(d3dptr);
	return result;
}


static const interface d3d9_interface =
{
	check_device_format,
	check_device_type,
	create_device,
	enum_adapter_modes,
	get_adapter_count,
	get_adapter_display_mode,
	get_adapter_identifier,
	get_adapter_mode_count,
	get_adapter_monitor,
	get_caps_dword,
	release
};



//============================================================
//  Direct3DDevice interfaces
//============================================================

static HRESULT device_begin_scene(device *dev)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	return IDirect3DDevice9_BeginScene(device);
}

static HRESULT device_clear(device *dev, DWORD count, const D3DRECT *rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	return IDirect3DDevice9_Clear(device, count, rects, flags, color, z, stencil);
}


static HRESULT device_create_offscreen_plain_surface(device *dev, UINT width, UINT height, D3DFORMAT format, D3DPOOL pool, surface **surface)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	return IDirect3DDevice9_CreateOffscreenPlainSurface(device, width, height, format, pool, (IDirect3DSurface9 **)surface, NULL);
}

static HRESULT device_create_effect(device *dev, const WCHAR *name, effect **effect)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;

	LPD3DXBUFFER buffer_errors = NULL;
	HRESULT hr = (*g_load_effect)(device, name, NULL, NULL, 0, NULL, (ID3DXEffect**)effect, &buffer_errors);
	if(FAILED(hr))
	{
		if(buffer_errors != NULL)
		{
			LPVOID compile_errors = buffer_errors->GetBufferPointer();
			printf("Unable to compile shader %s:\n%s\n", (const char*)name, (const char*)compile_errors);
		}
		else
		{
			printf("Unable to compile shader (unspecified reason)\n");
		}
	}

	return hr;
}


static HRESULT device_create_texture(device *dev, UINT width, UINT height, UINT levels, DWORD usage, D3DFORMAT format, D3DPOOL pool, texture **texture)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	return IDirect3DDevice9_CreateTexture(device, width, height, levels, usage, format, pool, (IDirect3DTexture9 **)texture, NULL);
}


static HRESULT device_create_vertex_buffer(device *dev, UINT length, DWORD usage, DWORD fvf, D3DPOOL pool, vertex_buffer **buf)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	return IDirect3DDevice9_CreateVertexBuffer(device, length, usage, fvf, pool, (IDirect3DVertexBuffer9 **)buf, NULL);
}


static HRESULT device_draw_primitive(device *dev, D3DPRIMITIVETYPE type, UINT start, UINT count)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	return IDirect3DDevice9_DrawPrimitive(device, type, start, count);
}


static HRESULT device_end_scene(device *dev)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	return IDirect3DDevice9_EndScene(device);
}


static HRESULT device_get_raster_status(device *dev, D3DRASTER_STATUS *status)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	return IDirect3DDevice9_GetRasterStatus(device, 0, status);
}


static HRESULT device_get_render_target(device *dev, DWORD index, surface **surface)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	return IDirect3DDevice9_GetRenderTarget(device, index, (IDirect3DSurface9 **)surface);
}


static HRESULT device_get_render_target_data(device *dev, surface *rendertarget, surface *destsurface)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	return IDirect3DDevice9_GetRenderTargetData(device, (IDirect3DSurface9 *)rendertarget, (IDirect3DSurface9 *)destsurface);
}


static HRESULT device_present(device *dev, const RECT *source, const RECT *dest, HWND override, RGNDATA *dirty, DWORD flags)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	if (flags != 0)
	{
		IDirect3DSwapChain9 *chain;
		HRESULT result = IDirect3DDevice9_GetSwapChain(device, 0, &chain);
		if (result == D3D_OK)
		{
			result = IDirect3DSwapChain9_Present(chain, source, dest, override, dirty, flags);
			IDirect3DSwapChain9_Release(chain);
			return result;
		}
	}
	return IDirect3DDevice9_Present(device, source, dest, override, dirty);
}


static ULONG device_release(device *dev)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	return IDirect3DDevice9_Release(device);
}


static HRESULT device_reset(device *dev, present_parameters *params)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	D3DPRESENT_PARAMETERS d3d9params;
	convert_present_params(params, &d3d9params);
	return IDirect3DDevice9_Reset(device, &d3d9params);
}


static void device_set_gamma_ramp(device *dev, DWORD flags, const D3DGAMMARAMP *ramp)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	IDirect3DDevice9_SetGammaRamp(device, 0, flags, ramp);
}


static HRESULT device_set_render_state(device *dev, D3DRENDERSTATETYPE state, DWORD value)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	return IDirect3DDevice9_SetRenderState(device, state, value);
}


static HRESULT device_set_render_target(device *dev, DWORD index, surface *surf)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	IDirect3DSurface9 *surface = (IDirect3DSurface9 *)surf;
	return IDirect3DDevice9_SetRenderTarget(device, index, surface);
}


static HRESULT device_create_render_target(device *dev, UINT width, UINT height, D3DFORMAT format, surface **surface)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	return IDirect3DDevice9_CreateRenderTarget(device, width, height, format, D3DMULTISAMPLE_NONE, 0, false, (IDirect3DSurface9 **)surface, NULL);
}


static HRESULT device_set_stream_source(device *dev, UINT number, vertex_buffer *vbuf, UINT stride)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	IDirect3DVertexBuffer9 *vertexbuf = (IDirect3DVertexBuffer9 *)vbuf;
	return IDirect3DDevice9_SetStreamSource(device, number, vertexbuf, 0, stride);
}


static HRESULT device_set_texture(device *dev, DWORD stage, texture *tex)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	IDirect3DBaseTexture9 *texture = (IDirect3DBaseTexture9 *)tex;
	return IDirect3DDevice9_SetTexture(device, stage, texture);
}


static HRESULT device_set_texture_stage_state(device *dev, DWORD stage, D3DTEXTURESTAGESTATETYPE state, DWORD value)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;

	// some state which was here got pushed into sampler state in D3D9
	switch ((DWORD)state)
	{
		case D3DTSS_ADDRESSU:
			return IDirect3DDevice9_SetSamplerState(device, stage, D3DSAMP_ADDRESSU, value);
		case D3DTSS_ADDRESSV:
			return IDirect3DDevice9_SetSamplerState(device, stage, D3DSAMP_ADDRESSV, value);
		case D3DTSS_BORDERCOLOR:
			return IDirect3DDevice9_SetSamplerState(device, stage, D3DSAMP_BORDERCOLOR, value);
		case D3DTSS_MAGFILTER:
			return IDirect3DDevice9_SetSamplerState(device, stage, D3DSAMP_MAGFILTER, value);
		case D3DTSS_MINFILTER:
			return IDirect3DDevice9_SetSamplerState(device, stage, D3DSAMP_MINFILTER, value);
		case D3DTSS_MIPFILTER:
			return IDirect3DDevice9_SetSamplerState(device, stage, D3DSAMP_MIPFILTER, value);
		case D3DTSS_MIPMAPLODBIAS:
			return IDirect3DDevice9_SetSamplerState(device, stage, D3DSAMP_MIPMAPLODBIAS, value);
		case D3DTSS_MAXMIPLEVEL:
			return IDirect3DDevice9_SetSamplerState(device, stage, D3DSAMP_MAXMIPLEVEL, value);
		case D3DTSS_MAXANISOTROPY:
			return IDirect3DDevice9_SetSamplerState(device, stage, D3DSAMP_MAXANISOTROPY, value);
		default:
			return IDirect3DDevice9_SetTextureStageState(device, stage, state, value);
	}
}


static HRESULT device_set_vertex_format(device *dev, D3DFORMAT format)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	return IDirect3DDevice9_SetFVF(device, format);
}


static HRESULT device_stretch_rect(device *dev, surface *source, const RECT *srcrect, surface *dest, const RECT *dstrect, D3DTEXTUREFILTERTYPE filter)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	IDirect3DSurface9 *ssurface = (IDirect3DSurface9 *)source;
	IDirect3DSurface9 *dsurface = (IDirect3DSurface9 *)dest;
	return IDirect3DDevice9_StretchRect(device, ssurface, srcrect, dsurface, dstrect, filter);
}


static HRESULT device_test_cooperative_level(device *dev)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	return IDirect3DDevice9_TestCooperativeLevel(device);
}


static const device_interface d3d9_device_interface =
{
	device_begin_scene,
	device_clear,
	device_create_offscreen_plain_surface,
	device_create_effect,
	device_create_texture,
	device_create_vertex_buffer,
	device_create_render_target,
	device_draw_primitive,
	device_end_scene,
	device_get_raster_status,
	device_get_render_target,
	device_get_render_target_data,
	device_present,
	device_release,
	device_reset,
	device_set_gamma_ramp,
	device_set_render_state,
	device_set_render_target,
	device_set_stream_source,
	device_set_texture,
	device_set_texture_stage_state,
	device_set_vertex_format,
	device_stretch_rect,
	device_test_cooperative_level
};



//============================================================
//  Direct3DSurface interfaces
//============================================================

static HRESULT surface_lock_rect(surface *surf, D3DLOCKED_RECT *locked, const RECT *rect, DWORD flags)
{
	IDirect3DSurface9 *surface = (IDirect3DSurface9 *)surf;
	return IDirect3DSurface9_LockRect(surface, locked, rect, flags);
}


static ULONG surface_release(surface *surf)
{
	IDirect3DSurface9 *surface = (IDirect3DSurface9 *)surf;
	return IDirect3DSurface9_Release(surface);
}


static HRESULT surface_unlock_rect(surface *surf)
{
	IDirect3DSurface9 *surface = (IDirect3DSurface9 *)surf;
	return IDirect3DSurface9_UnlockRect(surface);
}


static const surface_interface d3d9_surface_interface =
{
	surface_lock_rect,
	surface_release,
	surface_unlock_rect
};



//============================================================
//  Direct3DTexture interfaces
//============================================================

static HRESULT texture_get_surface_level(texture *tex, UINT level, surface **surface)
{
	IDirect3DTexture9 *texture = (IDirect3DTexture9 *)tex;
	return IDirect3DTexture9_GetSurfaceLevel(texture, level, (IDirect3DSurface9 **)surface);
}


static HRESULT texture_lock_rect(texture *tex, UINT level, D3DLOCKED_RECT *locked, const RECT *rect, DWORD flags)
{
	IDirect3DTexture9 *texture = (IDirect3DTexture9 *)tex;
	return IDirect3DTexture9_LockRect(texture, level, locked, rect, flags);
}


static ULONG texture_release(texture *tex)
{
	IDirect3DTexture9 *texture = (IDirect3DTexture9 *)tex;
	return IDirect3DTexture9_Release(texture);
}


static HRESULT texture_unlock_rect(texture *tex, UINT level)
{
	IDirect3DTexture9 *texture = (IDirect3DTexture9 *)tex;
	return IDirect3DTexture9_UnlockRect(texture, level);
}


static const texture_interface d3d9_texture_interface =
{
	texture_get_surface_level,
	texture_lock_rect,
	texture_release,
	texture_unlock_rect
};



//============================================================
//  Direct3DVertexBuffer interfaces
//============================================================

static HRESULT vertex_buffer_lock(vertex_buffer *vbuf, UINT offset, UINT size, VOID **data, DWORD flags)
{
	IDirect3DVertexBuffer9 *vertexbuf = (IDirect3DVertexBuffer9 *)vbuf;
	return IDirect3DVertexBuffer9_Lock(vertexbuf, offset, size, data, flags);
}


static ULONG vertex_buffer_release(vertex_buffer *vbuf)
{
	IDirect3DVertexBuffer9 *vertexbuf = (IDirect3DVertexBuffer9 *)vbuf;
	return IDirect3DVertexBuffer9_Release(vertexbuf);
}


static HRESULT vertex_buffer_unlock(vertex_buffer *vbuf)
{
	IDirect3DVertexBuffer9 *vertexbuf = (IDirect3DVertexBuffer9 *)vbuf;
	return IDirect3DVertexBuffer9_Unlock(vertexbuf);
}


static const vertex_buffer_interface d3d9_vertex_buffer_interface =
{
	vertex_buffer_lock,
	vertex_buffer_release,
	vertex_buffer_unlock
};



//============================================================
//  Direct3DEffect interfaces
//============================================================

uniform::uniform(effect *shader, const char *name, uniform_type type)
{
	m_shader = shader;
	m_type = type;
	m_next = NULL;
	m_prev = NULL;
	m_handle = m_shader->get_parameter(NULL, name);
	m_ival = 0;
	memset(m_vec, 0, sizeof(float) * 4);
	m_mval = NULL;
	m_texture = NULL;

	switch (type)
	{
	case UT_INT:
	case UT_FLOAT:
	case UT_MATRIX:
	case UT_SAMPLER:
		m_count = 1;
		break;
	case UT_VEC2:
		m_count = 2;
		break;
	case UT_VEC3:
		m_count = 3;
		break;
	case UT_VEC4:
		m_count = 4;
		break;
	default:
		m_count = 1;
		break;
	}
}

void uniform::set_next(uniform *next)
{
	m_next->set_prev(next);
	next->set_next(m_next);

	next->set_prev(this);
	m_next = next;
}

void uniform::set_prev(uniform *prev)
{
	m_prev->set_next(prev);
	prev->set_prev(m_prev);

	prev->set_next(this);
	m_prev = prev;
}

void uniform::set(float x, float y, float z, float w)
{
	m_vec[0] = x;
	m_vec[1] = y;
	m_vec[2] = z;
	m_vec[3] = w;
}

void uniform::set(float x, float y, float z)
{
	m_vec[0] = x;
	m_vec[1] = y;
	m_vec[2] = z;
}

void uniform::set(float x, float y)
{
	m_vec[0] = x;
	m_vec[1] = y;
}

void uniform::set(float x)
{
	m_vec[0] = x;
}

void uniform::set(int x)
{
	m_ival = x;
}

void uniform::set(matrix *mat)
{
	m_mval = mat;
}

void uniform::set(texture *tex)
{
	m_texture = tex;
}

void uniform::upload()
{
	switch(m_type)
	{
		case UT_INT:
			m_shader->set_int(m_handle, m_ival);
			break;
		case UT_FLOAT:
			m_shader->set_float(m_handle, m_vec[0]);
			break;
		case UT_VEC2:
		case UT_VEC3:
		case UT_VEC4:
			m_shader->set_vector(m_handle, m_count, m_vec);
			break;
		case UT_MATRIX:
			m_shader->set_matrix(m_handle, m_mval);
			break;
		case UT_SAMPLER:
			m_shader->set_texture(m_handle, m_texture);
			break;
	}
}

effect::effect(device *dev, const char *name, const char *path)
{
	IDirect3DDevice9 *device = (IDirect3DDevice9 *)dev;
	LPD3DXBUFFER buffer_errors = NULL;

	m_effect = NULL;
	m_valid = false;

	char name_cstr[1024];
	sprintf(name_cstr, "%s\\%s", path, name);
	TCHAR *effect_name = tstring_from_utf8(name_cstr);

	HRESULT hr = (*g_load_effect)(device, effect_name, NULL, NULL, 0, NULL, &m_effect, &buffer_errors);
	if(FAILED(hr))
	{
		if(buffer_errors != NULL)
		{
			LPVOID compile_errors = buffer_errors->GetBufferPointer();
			printf("Unable to compile shader: %s\n", (const char*)compile_errors);
		}
		else
		{
			printf("Unable to compile shader (unspecified reason)\n");
		}
	}
	else
	{
		m_valid = true;
	}

	osd_free(effect_name);
}

effect::~effect()
{
	m_effect->Release();
	m_effect = NULL;
}

void effect::begin(UINT *passes, DWORD flags)
{
	m_effect->Begin(passes, flags);
}

void effect::end()
{
	m_effect->End();
}

void effect::begin_pass(UINT pass)
{
	m_effect->BeginPass(pass);
}

void effect::end_pass()
{
	m_effect->EndPass();
}

void effect::set_technique(const char *name)
{
	m_effect->SetTechnique(name);
}

void effect::set_vector(D3DXHANDLE param, int count, float *vector)
{
	static D3DXVECTOR4 out_vector;
	if (count > 0)
		out_vector.x = vector[0];
	if (count > 1)
		out_vector.y = vector[1];
	if (count > 2)
		out_vector.z = vector[2];
	if (count > 3)
		out_vector.w = vector[3];
	m_effect->SetVector(param, &out_vector);
}

void effect::set_float(D3DXHANDLE param, float value)
{
	m_effect->SetFloat(param, value);
}

void effect::set_int(D3DXHANDLE param, int value)
{
	m_effect->SetInt(param, value);
}

void effect::set_matrix(D3DXHANDLE param, matrix *matrix)
{
	m_effect->SetMatrix(param, (D3DXMATRIX*)matrix);
}

void effect::set_texture(D3DXHANDLE param, texture *tex)
{
	m_effect->SetTexture(param, (IDirect3DTexture9*)tex);
}

D3DXHANDLE effect::get_parameter(D3DXHANDLE param, const char *name)
{
	return m_effect->GetParameterByName(param, name);
}

ULONG effect::release()
{
	return m_effect->Release();
}

//============================================================
//  set_interfaces
//============================================================

static void set_interfaces(base *d3dptr)
{
	d3dptr->d3d = d3d9_interface;
	d3dptr->device = d3d9_device_interface;
	d3dptr->surface = d3d9_surface_interface;
	d3dptr->texture = d3d9_texture_interface;
	d3dptr->vertexbuf = d3d9_vertex_buffer_interface;
}

};
