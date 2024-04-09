program Main;

uses
  Windows,
  System.SysUtils,
  uIndicador in 'uIndicador.pas';

function Check(resp: Integer): Boolean;
begin
  if resp > 0 then
    begin
      Writeln('SUCESSO.');
      Result := True;
    end
  else
    begin
      Writeln('ERRO.');
      Result := False;
    end;
end;

var
  indicador: uIndicador.Indicador;
  opc : integer;
  tara : integer;
  output: PAnsiChar;
  precisaoDecimal : integer;

begin
  indicador := uIndicador.Indicador.Create;
  try
    indicador.newIndicador();

    {if indicador.openSerial('COM4', 9600, 8, 1, 0) then} if indicador.openTcp('10.20.0.200', 502) then
    begin
      Writeln('Conexao aberta com sucesso.');

      opc := 1;

      while opc <> 0 do
      begin
        Write('1 - ZERAR' + sLineBreak +
              '2 - MOSTRAR DADOS' + sLineBreak +
              '3 - TARAR AUTOMATICO' + sLineBreak +
              '4 - TARAR MANUAL' + sLineBreak +
              '5 - GERAR RELATORIO COM AS 10K ULTIMAS IMPRESSOES' + sLineBreak +
              '0 - SAIR' + sLineBreak +
              'Opcao: ');
        Readln(opc);

        case opc of
          1:
          begin
            Check(indicador.zerar());
          end;
          2:
          begin
            precisaoDecimal := indicador.getPrecisaoDecimalPeso();
            if indicador.getStatusbalanca() >= 0 then
              begin
                if indicador.isModoContadoraDePecas() then
                  WriteLn('Pecas       : ', Format('%0:.0f', [indicador.getPesoLiquido()]))
                else
                  WriteLn('Peso Liquido: ', Format('%0:.' + IntToStr(precisaoDecimal) + 'f', [indicador.getPesoLiquido()]));
                WriteLn('Peso Bruto  : ', Format('%0:.' + IntToStr(precisaoDecimal) + 'f', [indicador.getPesoBruto()]));
                WriteLn('Tara        : ', Format('%0:.' + IntToStr(precisaoDecimal) + 'f', [indicador.getTara()]));
              end;

            Write('Status      : ');
            case indicador.getStatusBalanca() of
              -3:
                WriteLn('Peso em Excesso Positivo');
              -2:
                WriteLn('Peso em Excesso Negativo');
              0:
                WriteLn('Peso Estavel');
              1:
                WriteLn('Peso Instavel');
              2:
                WriteLn('Valor de Pico');
              3:
                WriteLn('Peso Medio');
              4:
                WriteLn('Aguardando');
              5:
                WriteLn('Perda de Comunicacao');
              6:
                WriteLn('Imprimindo');
              else
                WriteLn('Sem informacao');
            end;
            Write('Is Modo Contadora de Pecas?  ');
            if indicador.isModoContadoraDePecas() then
              WriteLn('SIM')
            else
              WriteLn('NAO');

            WriteLn('PROTOCOLO LIDER ', indicador.getProtocolo() + 1);
            Write('UNIDADE DE MEDIDA: ');
            case indicador.getUnidadeMedida() of
              2: WriteLn('KG');
              1: WriteLn('LB');
              0: WriteLn('KN');
            else
              WriteLn('Desconhecido');
            end;
          end;
          3:
          begin
            Check(indicador.tarar());
          end;
          4:
          begin
            Write('TARA: ');
            Readln(tara);
            Check(indicador.tararManual(tara));
          end;
          5:
          begin
            output := indicador.gerarRelatorio(0, '10000');
            WriteLn(output);
          end;
          0:
            Writeln('Saindo...');
          else
            Writeln('Opcao invalida.');
        end;

      end;

    end
    else
    begin
      Writeln('Nao foi possivel abrir a conexao.');
    end;

    indicador.deletar();
  except
    on E: Exception do
      Writeln(E.ClassName, ': ', E.Message);
  end;
  indicador.Free();
  Sleep(2000);
end.
