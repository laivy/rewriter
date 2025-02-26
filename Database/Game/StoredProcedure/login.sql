USE [game]
GO

DROP PROCEDURE IF EXISTS [dbo].[login]
GO

CREATE PROCEDURE [dbo].[login]
	@name NVARCHAR(16),
	@password NVARCHAR(16)
AS
BEGIN
	DECLARE @account_id INT = -1

	SELECT @account_id = [id]
	FROM [dbo].[account]
	WHERE [name] = @name AND [password] = HASHBYTES('SHA2_256', @password)

	-- 아이디 또는 비밀번호가 잘못됨
	IF @account_id = -1
		RETURN 1

	-- 정지 당한 계정
	IF EXISTS (
		SELECT 1
		FROM [dbo].[ban_account]
		WHERE [account_id] = @account_id AND GETDATE() BETWEEN [start_date] AND [end_date]
	)
		RETURN 2

	RETURN 0
END
GO
