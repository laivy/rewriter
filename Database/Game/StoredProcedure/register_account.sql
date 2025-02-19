USE [game]
GO

DROP PROCEDURE IF EXISTS [dbo].[register_account];
GO

CREATE PROCEDURE [dbo].[register_account]
	@id NVARCHAR(16),
	@pw NVARCHAR(16)
AS
BEGIN
	INSERT INTO [dbo].[account]
	VALUES (@id, @pw, GETDATE())
END
GO
