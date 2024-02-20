#define RGFW_IMPLEMENTATION
#define RGFW_DIRECTX
#include "RGFW.h"

#include <D3dx9math.h>

typedef struct {FLOAT X, Y, Z;} Vertex;

int main() {
    RGFW_window* win = RGFW_createWindow("name", 0, 0, 500, 500, RGFW_CENTER);
    RGFW_window_makeCurrent(win);

    RGFW_directXinfo dxInfo = *RGFW_getDirectXInfo();

    // Set viewport
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = win->w;
    viewport.Height = win->h;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    dxInfo.pDeviceContext->lpVtbl->RSSetViewports(dxInfo.pDeviceContext, 1, &viewport);
    
    Vertex vertices[] = {
        { 0.0f, 0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { -0.5f, -0.5f, 0.0f}
    };
    
    ID3D11Buffer* pVertexBuffer;

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    dxInfo.pDeviceContext->lpVtbl->IASetVertexBuffers( dxInfo.pDeviceContext, 0, 1, &pVertexBuffer, &stride, &offset);

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(Vertex) * 3;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    dxInfo.pDevice->lpVtbl->CreateBuffer(dxInfo.pDevice, &bd, NULL, &pVertexBuffer);

    D3D11_MAPPED_SUBRESOURCE ms;
    dxInfo.pDeviceContext->lpVtbl->Map(dxInfo.pDeviceContext, (ID3D11Resource*)pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
    memcpy(ms.pData, vertices, sizeof(vertices));
    dxInfo.pDeviceContext->lpVtbl->Unmap(dxInfo.pDeviceContext, (ID3D11Resource*)pVertexBuffer, 0);   

    // Compile shaders
    ID3D10Blob* pVertexShaderBlob = NULL;
    ID3D10Blob* pPixelShaderBlob = NULL;
    ID3D10Blob* pErrorBlob = NULL;
    D3DCompileFromFile(L"triangle.hlsl", NULL, NULL, "VS", "vs_5_0", 0, 0, &pVertexShaderBlob, &pErrorBlob);
    D3DCompileFromFile(L"triangle.hlsl", NULL, NULL, "PS", "ps_5_0", 0, 0, &pPixelShaderBlob, &pErrorBlob);

    // Create shaders
    ID3D11VertexShader* pVertexShader;
    ID3D11PixelShader* pPixelShader;
    dxInfo.pDevice->lpVtbl->CreateVertexShader(dxInfo.pDevice, pVertexShaderBlob->lpVtbl->GetBufferPointer(pVertexShaderBlob), pVertexShaderBlob->lpVtbl->GetBufferSize(pVertexShaderBlob), NULL, &pVertexShader);
    dxInfo.pDevice->lpVtbl->CreatePixelShader(dxInfo.pDevice, pPixelShaderBlob->lpVtbl->GetBufferPointer(pPixelShaderBlob), pPixelShaderBlob->lpVtbl->GetBufferSize(pPixelShaderBlob), NULL, &pPixelShader);
    
    dxInfo.pDeviceContext->lpVtbl->VSSetShader(dxInfo.pDeviceContext, pVertexShader, 0, 0);
    dxInfo.pDeviceContext->lpVtbl->PSSetShader(dxInfo.pDeviceContext, pPixelShader, 0, 0);

    // Set input layout
    ID3D11InputLayout* pInputLayout;
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    
    dxInfo.pDevice->lpVtbl->CreateInputLayout(dxInfo.pDevice, layout, 1, pVertexShaderBlob->lpVtbl->GetBufferPointer(pVertexShaderBlob), pVertexShaderBlob->lpVtbl->GetBufferSize(pVertexShaderBlob), &pInputLayout);
    dxInfo.pDeviceContext->lpVtbl->IASetInputLayout(dxInfo.pDeviceContext, pInputLayout);

    for (;;) {
        RGFW_window_checkEvent(win); // NOTE: checking events outside of a while loop may cause input lag 

        if (win->event.type == RGFW_quit || RGFW_isPressedI(win, RGFW_Escape))
            break;

        float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        dxInfo.pDeviceContext->lpVtbl->ClearRenderTargetView(dxInfo.pDeviceContext, win->renderTargetView, clearColor);

        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        dxInfo.pDeviceContext->lpVtbl->IASetVertexBuffers(dxInfo.pDeviceContext, 0, 1, &pVertexBuffer, &stride, &offset);

        dxInfo.pDeviceContext->lpVtbl->IASetPrimitiveTopology(dxInfo.pDeviceContext, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        dxInfo.pDeviceContext->lpVtbl->IASetInputLayout(dxInfo.pDeviceContext, pInputLayout);

        dxInfo.pDeviceContext->lpVtbl->VSSetShader(dxInfo.pDeviceContext, pVertexShader, NULL, 0);
        dxInfo.pDeviceContext->lpVtbl->PSSetShader(dxInfo.pDeviceContext, pPixelShader, NULL, 0);

        dxInfo.pDeviceContext->lpVtbl->Draw(dxInfo.pDeviceContext, 3, 0);

        RGFW_window_swapBuffers(win);
    }

    RGFW_window_close(win);
}