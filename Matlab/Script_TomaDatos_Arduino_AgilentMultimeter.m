clear
clc

max = 100; %numero de muestras
altura = 200; %Altura de la grafica
%Datos = zeros(2,max);

newobjs = instrfind;
if isempty(newobjs) == 0
    fclose(newobjs);
    delete(newobjs);
    clear newobjs
end

%%%%%%%%%GRAFICA%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure('Name','Grafica Corriente')
title('Comparacion de mediciones')
xlabel('Numero de muestra (T=0.5s)')
ylabel('Tension(V)')
grid off
hold on
ylim([0 altura]);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%Configuracion Agilent 34405A%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    DutAddr = 'USB0::0x0957::0x0618::MY51490014::0::INSTR';
    %connects with instrument and configures
    myDmm = visa('agilent',DutAddr);
    set(myDmm,'EOSMode','read&write');
    set(myDmm,'EOSCharCode','LF') ;
    fopen(myDmm);     
    
    %Configure for DCV 100V range, 100uV resolution
    fprintf(myDmm,'CONF:VOLT:DC 10, 0.0001');
    
    %Configure for DCI 10A range, 100uA resolution
    %fprintf(myDmm,'CONF:CURR:DC 10, 0.0001');
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Confiuracion para Arduino
arduino = serial('COM4','BaudRate',115200);
fopen(arduino);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%Funcion para Adquirir de datos %%%%%%%%%%%%%%%%%%%%%%%%
    for i = 1:max
    Datos(i,2)=(fscanf(arduino,'%f'))/100;
        
    fprintf(myDmm,'READ?');
    DCVResult = fscanf(myDmm);
    Datos(i,1) = str2double(DCVResult)*1000;
    
    xlim([0 i]);
    plot(Datos(:,1),'r') %Agilent Multimeter
    plot(Datos(:,2),'b--o') %Arduino Mega 
    
    drawnow
    end    
fclose(arduino);

%Calcula el Error Cuadratico Medio
M=Datos(:,1);
P=Datos(:,2);
P_raya=sum(P)/max;
M_raya=sum(M)/max;
ECM =(sum((P-M).^2)/(P_raya*M_raya))/max;

%%Legenda en grafica
textposx = (max/10)*7;
textposy = (altura/10)*8;
text(textposx,textposy,strcat('ECM=',num2str(ECM)))
legend('AGILENT','ATMEGA')

%%Guarda los datos en un archivo
data = fopen('prueba01.csv','w');
fprintf(data,'%s ; %s \n','Agilent','Arduino');
fclose(data);
%dlmwrite('prueba01.csv',Datos,'precision','%.4f','delimiter',';','-append');
%end
xlswrite('tensionA1.xlsx',Datos)

%Encuentra la curva de mejor ajuste
polyfit(P,M,1) %Curva de mejor ajuste grado 1
