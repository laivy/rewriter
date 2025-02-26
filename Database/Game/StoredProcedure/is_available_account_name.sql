USE [game]
GO

DROP PROCEDURE IF EXISTS [dbo].[is_available_account_name]
GO

CREATE PROCEDURE [dbo].[is_available_account_name]
	@name NVARCHAR(16)
AS
BEGIN
	IF EXISTS (SELECT 1 FROM [dbo].[account] WHERE [name] = @name)
		RETURN 1
	RETURN 0
END
GO
