unit uIndicador;

interface

uses
  Windows,
  System.SysUtils;

type
  serialF       = function ( port :PAnsiChar ; baudRate : integer ; dataBits : integer ; stopBits : integer ; parity : integer ): integer ; cdecl;
  tcpF          = function ( ip   :PAnsiChar ; port : integer ): integer ; cdecl;
  isOpenF       = function ( ) : integer ; cdecl;
  closeF        = function ( ) : integer ; cdecl;
  zerarF        = function ( ) : integer ; cdecl;
  tararF        = function ( ) : integer ; cdecl;
  tararManualF  = function ( tara : integer ) : integer ; cdecl;

  getPesoLiquidoF         = function () : double  ; cdecl;
  getPesoBrutoF           = function () : double  ; cdecl;
  getTaraF                = function () : double  ; cdecl;
  getPrecisaoDecimalPesoF = function () : integer ; cdecl;
  getStatusBalancaF       = function () : integer ; cdecl;
  getUnidadeMedidaF       = function () : integer ; cdecl;
  isModoContadoraDePecasF = function () : integer ; cdecl;
  getProtocoloF           = function () : integer ; cdecl;
  getStatusBateriaIndF    = function () : integer ; cdecl;
  getNivelBateriaIndF     = function () : integer ; cdecl;
  getNivelBateriaTx1F     = function () : integer ; cdecl;
  getNivelBateriaTx2F     = function () : integer ; cdecl;
  gerarRelatorioF         = function ( tipo : integer ; input : PAnsiChar ) : PAnsiChar; cdecl;

type Indicador = class
  public
    constructor newIndicador;
    destructor deletar;

    function openSerial ( port:RawByteString; baudRate:integer; dataBits:integer; stopBits:integer; parity:integer )  : Boolean;
    function openTcp    ( ip:RawByteString; port:integer )        : Boolean;
    function isOpen     ( ) : Boolean;
    function close      ( ) : Boolean;
    function zerar      ( ) : integer;
    function tarar      ( ) : integer;
    function tararManual( tara:integer ) : integer;
    function gerarRelatorio ( tipo:integer; input:PAnsiChar ) : PAnsiChar;

    function getPesoLiquido                 ( ) : double;
    function getPesoBruto                   ( ) : double;
    function getTara                        ( ) : double;
    function getPrecisaoDecimalPeso         ( ) : integer;
    function getStatusBalanca               ( ) : integer;
    function getUnidadeMedida               ( ) : integer;
    function isModoContadoraDePecas         ( ) : Boolean;
    function getProtocolo                   ( ) : integer;
    function getStatusBateriaIndicador      ( ) : integer;
    function getNivelBateriaIndicador       ( ) : integer;
    function getNivelBateriaTransmissorTx1  ( ) : integer;
    function getNivelBateriaTransmissorTx2  ( ) : integer;
end;

var
  hDll        : THandle;
  openCom     : serialF;
  openRede    : tcpF;
  conexaoOn   : isOpenF;
  fechar      : closeF;
  zera        : zerarF;
  taraAut     : tararF;
  taraMan     : tararManualF;
  relatorio   : gerarRelatorioF;

  pesoLiquido         : getPesoLiquidoF;
  pesoBruto           : getPesoBrutoF;
  tara                : getTaraF;
  precisaoDecimalPeso : getPrecisaoDecimalPesoF;
  statusBalanca       : getStatusBalancaF;
  unidadeMedida       : getUnidadeMedidaF;
  modoContadoraPecas  : isModoCOntadoraDePecasF;
  protocolo           : getProtocoloF;
  statusBateriaInd    : getStatusBateriaIndF;
  nivelBateriaInd     : getNivelBateriaIndF;
  nivelBateriaTx1     : getNivelBateriaTx1F;
  nivelBateriaTx2     : getNivelBateriaTx2F;

implementation

constructor Indicador.newIndicador;
begin
  hDll := LoadLibrary('../../../../../lib/x86/indicadorLider.dll');
  if hDll <> 0 then
  begin
    openCom     := GetProcAddress(hDll, 'openSerial');
    openRede    := GetProcAddress(hDll, 'openTcp');
    conexaoOn   := GetProcAddress(hDll, 'isOpen');
    fechar      := GetProcAddress(hDll, 'close');
    zera        := GetProcAddress(hDll, 'zerar');
    taraAut     := GetProcAddress(hDll, 'tarar');
    taraMan     := GetProcAddress(hDll, 'tararManual');
    relatorio   := GetProcAddress(hDll, 'gerarRelatorio');

    pesoLiquido         := GetProcAddress(hDll, 'getPesoLiquido');
    pesoBruto           := GetProcAddress(hDll, 'getPesoBruto');
    tara                := GetProcAddress(hDll, 'getTara');
    precisaoDecimalPeso := GetProcAddress(hDll, 'getPrecisaoDecimalPeso');
    statusBalanca       := GetProcAddress(hDll, 'getStatusBalanca');
    unidadeMedida       := GetProcAddress(hDll, 'getUnidadeMedida');
    modoContadoraPecas  := GetProcAddress(hDll, 'isModoContadoraDePecas');
    protocolo           := GetProcAddress(hDll, 'getProtocolo');
    statusBateriaInd    := GetProcAddress(hDll, 'getStatusBateriaIndicador');
    nivelBateriaInd     := GetProcAddress(hDll, 'getNivelBateriaIndicador');
    nivelBateriaTx1     := GetProcAddress(hDll, 'getNivelBateriaTransmissorTx1');
    nivelBateriaTx2     := GetProcAddress(hDll, 'getNivelBateriaTransmissorTx2');
  end
  else
  begin
    Writeln('Error code: ' + IntToStr(GetLastError()));
  end;
end;

destructor Indicador.deletar;
begin
  if isOpen() then
    close();
  FreeLibrary(hDll);
end;

function Indicador.openSerial(port:RawByteString; baudRate:integer; dataBits:integer; stopBits:integer; parity:integer): Boolean;
begin
  if openCom(PAnsiChar(PAnsiString(port)), baudRate, dataBits, stopBits, parity) >= 1 then
    result := True
  else
    result := False;
end;

function Indicador.openTcp(ip: RawByteString; port: integer): Boolean;
begin
  if openRede(PAnsiChar(PAnsiString(ip)), port) >= 1 then
    result := True
  else
    result := False;
end;

function Indicador.isOpen: Boolean;
begin
  if conexaoOn >= 1 then
    result := True
  else
    result := False;
end;

function Indicador.close: Boolean;
begin
  if fechar() >= 1 then
    result := True
  else
    result := False;
end;

function Indicador.zerar: Integer;
begin
  result := zera();
end;

function Indicador.tarar( ): Integer;
begin
  result := taraAut();
end;

function Indicador.tararManual(tara:integer): Integer;
begin
  result := taraMan(tara);
end;

function Indicador.gerarRelatorio(tipo: Integer; input: PAnsiChar): PAnsiChar;
begin
  result := relatorio(tipo, input);
end;

function Indicador.getPesoLiquido: Double;
begin
  result := pesoLiquido();
end;

function Indicador.getPesoBruto: Double;
begin
  result := pesoBruto();
end;

function Indicador.getTara: Double;
begin
  result := tara();
end;

function Indicador.getPrecisaoDecimalPeso: Integer;
begin
  result := precisaoDecimalPeso();
end;

function Indicador.getStatusBalanca: Integer;
begin
  result := statusBalanca();
end;

function Indicador.getUnidadeMedida: Integer;
begin
  result := unidadeMedida();
end;

function Indicador.isModoContadoraDePecas: Boolean;
begin
  if modoContadoraPecas >= 1 then
    result := True
  else
    result := False;
end;

function Indicador.getProtocolo: Integer;
begin
  result := protocolo();
end;

function Indicador.getStatusBateriaIndicador: Integer;
begin
  result := statusBateriaInd();
end;

function Indicador.getNivelBateriaIndicador: Integer;
begin
  result := nivelBateriaInd();
end;

function Indicador.getNivelBateriaTransmissorTx1: Integer;
begin
  result := nivelBateriaTx1();
end;

function Indicador.getNivelBateriaTransmissorTx2: Integer;
begin
  result := nivelBateriaTx2();
end;

end.
