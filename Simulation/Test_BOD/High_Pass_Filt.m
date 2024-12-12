% Đường dẫn file
file_path = 'D:\OneDrive\Disk_2\MEASURE-POWER-AC\Simulation\Test_BOD\High-Pass_Filt.bod';

fid = fopen(file_path, 'r');
line_num = 0;

frequency = [];
gain_db = [];
gain_linear = [];
phase_deg = [];

while ~feof(fid)
    line = strtrim(fgetl(fid)); % Đọc từng dòng và loại bỏ khoảng trắng
    
    % Bỏ qua dòng không chứa số (tiêu đề, dấu gạch ngang, v.v.)
    if isempty(line) || startsWith(line, '-') || ~contains(line, 'e')
        continue;
    end
    
    % Phân tích dòng dữ liệu
    data = sscanf(line, '%f %f %f %f');
    if length(data) == 4
        frequency(end+1) = data(1); % Cột 1: Frequency
        gain_db(end+1) = data(2);   % Cột 2: Gain (dB)
        gain_linear(end+1) = data(3); % Cột 3: Gain (Linear)
        phase_deg(end+1) = data(4); % Cột 4: Phase
    end
end

fclose(fid);

% Vẽ biểu đồ sau khi xử lý
figure;

% Subplot 1: Gain (dB)
subplot(2, 1, 1); % Tạo vùng đầu tiên (2 hàng, 1 cột, phần 1)
semilogx(frequency, gain_db, 'b', 'LineWidth', 1.5);
grid on;
xlabel('Frequency (Hz)');
ylabel('Gain (dB)');
title('Bode Plot - Gain (dB)');

% Subplot 2: Phase (Degrees)
subplot(2, 1, 2); % Tạo vùng thứ hai (2 hàng, 1 cột, phần 2)
semilogx(frequency, phase_deg, 'r', 'LineWidth', 1.5);
grid on;
xlabel('Frequency (Hz)');
ylabel('Phase (Degrees)');
title('Bode Plot - Phase');