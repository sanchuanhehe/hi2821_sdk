usage: sec_tool.py [-h] --gid GID [--iter ITER] --salt SALT --plaintext
                   PLAINTEXT --iv IV

options:
  -h, --help            show this help message and exit
  --gid GID             eFuse�е�GID, 64λ��ʮ�������ַ���, ��: "0102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20"
  --iter ITER           ��Կ������������, Ĭ��1024��.
  --salt SALT           ��Կ�����õ���ֵ, 32λ��ʮ�����ƴ�, ��ȫ�����, ��: "5AB7CD2BA2B3CCFD98C7DFE0BB365F0D"
  --plaintext PLAINTEXT �����ܵ������ַ���, Ҫ����ʮ�����ƴ�, ����32λ����.
  --iv IV               �����õĳ�ʼ����, 32λ��ʮ�����ƴ�, ��ȫ�����, ��: "5AB7CD2BA2B3CCFD98C7DFE0BB365F0D"
